#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <cstring>
#include <locale>
#include <OpenEXR/ImfRgbaFile.h>

#include <unistd.h>
#include <sys/ioctl.h>

#include "math/transform.hpp"
#include "math/util.hpp"
#include "math/random.hpp"
#include "math/sampler.hpp"
#include "tracer/camera.hpp"
#include "tracer/spectrum.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_inf_spheres.hpp"
#include "tracer/shapes/de_mandelbulb.hpp"
#include "tracer/scene.hpp"
#include "tracer/point_light.hpp"
#include "parser.hpp"

using namespace tracer;
using namespace math;

#define U_BLOCK L'\u25b0'
#define U_DOT L'\u00b7'
#define U_BRAILLE L'\u28AB'

int term_columns() {
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  return ws.ws_col;
}

wchar_t progress_string[256];

void update_progress(Float progress) {
  const int n_cols = term_columns();
  const int bar_width = n_cols * 3 / 4 - 2;
  const int finished = std::ceil(progress * bar_width);
  int offset = 0;
  if (progress < 1.0f) {
    progress_string[offset++] = U_BRAILLE + std::rand() % 20;
    progress_string[offset++] = L' ';
  }
  progress_string[offset++] = L'[';
  for (int i = 0; i < finished; ++i) {
    progress_string[offset + i] = U_BLOCK;
  }
  offset += finished;
  for (int i = 0; i < bar_width - finished; ++i) {
    progress_string[offset + i] = U_DOT;
  }
  offset += bar_width - finished;
  progress_string[offset++] = L']';
  progress_string[offset++] = L' ';
  offset += std::swprintf(progress_string + offset, 255 - offset, L"%.0f%%", progress * 100);
  progress_string[offset++] = L' ';
  progress_string[offset++] = L' ';
  progress_string[offset++] = L' ';
  std::wcout << L"\r" << progress_string << std::flush;
  std::wcout << std::flush;
}

int main(int argc, char** argv) {

  setlocale(LC_CTYPE, ""); // assume that LC supports unicode
  std::wcout << L"\r" << std::flush;
  std::srand(time(NULL));

  std::string output_file_name("output.exr");
  std::string scene_file_name;

  int arg_n_threads = 0;
  bool verbose = true;
  bool show_depth = false;

  if (argc > 1) {
    int n_sub_args = 0;
    for (int i = 1; i < argc; ++i) {
      if (argv[i][0] == '-') {
        if (!std::strcmp(argv[i], "-j")) {
          n_sub_args += 1;
          if (i + 1 < argc) {
            arg_n_threads = std::atoi(argv[i+1]);
            if (arg_n_threads < 1) {
              std::cerr << "error: number of render workers should be greater than 0" << std::endl;
              return 1;
            }
          } else {
            std::cerr << "error: please specify number of render workers" << std::endl;
            return 1;
          }
        } else if (!std::strcmp(argv[i], "-o")) {
          n_sub_args += 1;
          if (i + 1 < argc) {
            output_file_name = argv[i+1];
          } else {
            std::cerr << "error: please specify output file name" << std::endl;
            return 1;
          }
        } else if (!std::strcmp(argv[i], "--quiet")) {
          verbose = 0;
          std::cout.setstate(std::ios::failbit);
          std::wcout.setstate(std::ios::failbit);
        } else if (!std::strcmp(argv[i], "-d") || !std::strcmp(argv[i], "--depth")) {
          show_depth = true;
        } else if (!std::strcmp(argv[i], "-h") || !std::strcmp(argv[i], "--help")) {
          std::cerr <<
            "usage: ftracer [ -h | -j workers | -d | -o output | --help | --quiet | --depth ] SCENE"
            << std::endl;
          return 0;
        } else {
          std::cerr << "error: unknown option " << "`" << argv[i] << "'" << std::endl;
          return 1;
        } /* if strcmp */
      } /* if -option */
      else {
        scene_file_name = argv[i];
      }
      i += n_sub_args;
      n_sub_args = 0;
    } /* for i */
  } /* if argc */
  
  int n_threads = arg_n_threads ? arg_n_threads : std::thread::hardware_concurrency();
  std::wcout << "* Detected " << std::thread::hardware_concurrency()
    << " logical cores" << std::endl;

  parser scene_parser;
  std::shared_ptr<scene> main_scene;
  render_params params;

  if (scene_file_name.empty()) {
    std::cerr << "error: specify YAML scene file" << std::endl;
    return 1;
  }

  try {
    main_scene = scene_parser.load_scene(scene_file_name, &params);
  } catch (const std::exception& e) {
    std::cerr << "error: " << e.what() << std::endl;
    return 1;
  }

  // override parameters with the ones provided by the command line
  params.n_workers = n_threads;
  params.show_depth = show_depth;

  // setup rendering
  render_profile profile;

  // render
  std::wcout << L"* Rendering scene with " << main_scene->shapes.size() << L" shapes and "
    << main_scene->light_sources.size() << " light sources..." << std::endl;

  auto ird_rgb = main_scene->render(
      params, &profile, verbose ? &update_progress : nullptr
      );
  auto ird_rgb_exr = std::unique_ptr<Imf::Rgba>(new Imf::Rgba[params.img_res.x * params.img_res.y]);

  std::wcout << std::endl;
  std::wcout << "* " << profile.view_counter << " viewing rays hit" << std::endl;
  std::wcout << "* " << profile.shadow_counter << " shadow rays hit" << std::endl;

  // write to file
  for (size_t i = 0; i < ird_rgb->size(); ++i) {
    ird_rgb_exr.get()[i].r = ird_rgb->at(i).r();
    ird_rgb_exr.get()[i].g = ird_rgb->at(i).g();
    ird_rgb_exr.get()[i].b = ird_rgb->at(i).b();
  }

  Imf::RgbaOutputFile exr(
      output_file_name.c_str(),
      params.img_res.x,
      params.img_res.y,
      Imf::WRITE_RGB
      );
  exr.setFrameBuffer(ird_rgb_exr.get(), 1, params.img_res.x);
  exr.writePixels(params.img_res.y);

  return 0;
}
