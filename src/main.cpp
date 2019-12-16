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
#include "tracer/bvh_tree.hpp"
#include "parser.hpp"

using namespace tracer;
using namespace math;

#define U_BLOCK   L'\u25b0'
#define U_DOT     L'\u00b7'
#define U_BRAILLE L'\u28AB'

int term_columns() {
  struct winsize ws;
  ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
  return ws.ws_col;
}

wchar_t progress_string[256];

int format_duration(wchar_t* wstr, size_t s) {
  int n_written;
  if (s == 0) {
    n_written = 0;
  } else if (s < 60) {
    n_written = std::swprintf(wstr, 15, L"eta %lus", s);
  } else if (s < 3600) {
    size_t m = s / 60;
    s -= m * 60;
    n_written = std::swprintf(wstr, 15, L"eta %lum%lus", m, s);
  } else {
    size_t m = s / 60;
    size_t h = m / 60;
    m -= h * 60;
    s -= (m + h * 60) * 60;
    n_written = std::swprintf(wstr, 15, L"eta %luh%lum%lus", h, m, s);
  }
  return n_written;
}

void update_progress(Float progress, size_t eta) {
  const int n_cols = term_columns();
  const int bar_width = n_cols * 0.6f - 2;
  const int finished = std::ceil(progress * bar_width);
  int offset = 0;
  if (progress < 1.0f) {
    progress_string[offset++] = U_BRAILLE + std::rand() % 64;
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

  wchar_t eta_str[16];
  int len = format_duration(eta_str, eta);
  std::wcscpy(progress_string + offset, eta_str);
  offset += len;
  while (offset < n_cols) progress_string[offset++] = L' ';

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
  bool show_normal = false;

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
        } else if (!std::strcmp(argv[i], "-n") || !std::strcmp(argv[i], "--normal")) {
          show_normal = true;
        } else if (!std::strcmp(argv[i], "-h") || !std::strcmp(argv[i], "--help")) {
          std::cerr <<
            "usage: ftracer [ -h | -j workers | -d | -n | -o output"
            " | --help | --quiet | --depth | --normal ]"
            " SCENE"
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
  params.n_workers    = n_threads;
  params.show_depth   = show_depth;
  params.show_normal  = show_normal;

  // setup rendering
  render_profile profile;

  // render
  std::wcout << L"* Rendering scene with " << main_scene->shapes.n_shapes()
    << L" shapes..." << std::endl;

  auto ird_rgb = main_scene->render(
      params, &profile, verbose ? &update_progress : nullptr
      );
  auto ird_rgb_exr = std::unique_ptr<Imf::Rgba>(
      new Imf::Rgba[params.img_res.x * params.img_res.y]
      );

  std::wcout << std::endl;
  if (profile.time_elapsed > 0) {
    wchar_t elapsed_str[16];
    format_duration(elapsed_str, profile.time_elapsed);
    std::wcout << "* Total time of " << elapsed_str + 4 << std::endl;
  } else {
    std::wcout << "* Total time of 0s" << std::endl;
  }

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
