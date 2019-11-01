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
#include "tracer/camera.hpp"
#include "tracer/spectrum.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_inf_spheres.hpp"
#include "tracer/shapes/de_mandelbulb.hpp"
#include "tracer/scene.hpp"
#include "tracer/point_light.hpp"

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
  const int finished = progress * bar_width;
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

  int arg_n_threads = 0;
  if (argc >= 3) {
    if (std::string(argv[1]) == "-j") {
      arg_n_threads = std::atoi(argv[2]);
    }
  }

  //const vector2i img_res(3840, 2160); // 4k ftw
  //const vector2i img_res(3840 * 2, 2160 * 2);
  const vector2i img_res(1920, 1080);
  //const vector2i img_res(300, 300);
  const vector2f ndc_res(1, 1);
  const vector3f eye_pos(0, -2, 4);

  //shapes::de_sphere sphere(tf::transform(), 0.5f);
  //shapes::de_mandelbulb sphere(tf::rotate({ 1.0f, 0.0f, 0.0f }, math::radians(90)).mat);

  const rgb_spectrum surface_rgb(1.0, 1.0, 1.0);
  
  int n_threads = arg_n_threads ? arg_n_threads : std::thread::hardware_concurrency();
  std::wcout << L"* Detected " << std::thread::hardware_concurrency()
    << L" logical cores" << std::endl;

  auto main_scene = std::unique_ptr<scene>(new scene());
  render_profile profile;
  render_params params;

  // setup scene
  main_scene->camera = std::shared_ptr<camera::camera>(new camera::persp(
        tf::look_at(vector3f(0, 0, 0), eye_pos, vector3f(0, 1, 0)),
        img_res,
        ndc_res,
        0.1,
        1000,
        math::radians(60),
        Float(img_res.x) / img_res.y
        )
      );
  main_scene->shapes = {
      //std::shared_ptr<shape>(new shapes::de_sphere(tf::translate({ 2, 0, 0 }), 0.5f)),
      //std::shared_ptr<shape>(new shapes::de_sphere(tf::translate({ -2, 0, 0 }), 2.0f)),
      //std::shared_ptr<shape>(new shapes::de_sphere(tf::translate({ 8, 0, 0 }), 2.0f)),
      //std::shared_ptr<shape>(new shapes::de_inf_spheres(tf::identity, 0.8f, 5.2f))
      std::shared_ptr<shape>(
          new shapes::de_mandelbulb(tf::rotate({ 1.0f, 0.0f, 0.0f }, math::radians(90)))
          ),
  };
  main_scene->light_sources = {
    std::shared_ptr<light_source>(
        new point_light(tf::translate({ 1, -2, 4 }), rgb_spectrum(0.7f))
        ),
  };

  params.shadow_bias = 5e-4;
  params.eye_position = eye_pos;
  params.surface_rgb = { 1.0f, 1.0f, 1.0f };
  params.Kd = 0.8;
  params.Ks = 0.5;
  params.Es = 32;

  // render
  std::wcout << L"* Rendering scene with " << main_scene->shapes.size() << L" shapes and "
    << main_scene->light_sources.size() << " light sources..." << std::endl;

  auto ird_rgb = main_scene->render(params, img_res, n_threads, &profile, &update_progress);
  auto ird_rgb_exr = std::unique_ptr<Imf::Rgba>(new Imf::Rgba[img_res.x * img_res.y]);

  std::wcout << std::endl;
  std::wcout << L"* " << profile.view_counter << " viewing rays hit" << std::endl;
  std::wcout << L"* " << profile.shadow_counter << " shadow rays hit" << std::endl;

  // write to file
  for (size_t i = 0; i < ird_rgb->size(); ++i) {
    ird_rgb_exr.get()[i].r = ird_rgb->at(i).r();
    ird_rgb_exr.get()[i].g = ird_rgb->at(i).g();
    ird_rgb_exr.get()[i].b = ird_rgb->at(i).b();
  }

  Imf::RgbaOutputFile exr("output.exr", img_res.x, img_res.y, Imf::WRITE_RGB);
  exr.setFrameBuffer(ird_rgb_exr.get(), 1, img_res.x);
  exr.writePixels(img_res.y);

  return 0;
}
