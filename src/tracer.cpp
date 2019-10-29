#include <iostream>
#include <memory>
#include <thread>
#include <mutex>
#include <OpenEXR/ImfRgbaFile.h>

#include "math/transform.hpp"
#include "math/util.hpp"
#include "tracer/camera.hpp"
#include "tracer/spectrum.hpp"
#include "tracer/shapes/de_sphere.hpp"
#include "tracer/shapes/de_inf_spheres.hpp"
#include "tracer/shapes/de_mandelbulb.hpp"

using namespace tracer;
using namespace math;

struct light_source {
  const point3f position;
  const rgb_spectrum rgb;
};

// Blinn-phong
rgb_spectrum blinn_phong_brdf(
    const light_source& light,
    const point3f& surface_point,
    const rgb_spectrum& surface_rgb,
    const normal3f& normal,
    const point3f& eye,
    Float Kd,
    Float Ks,
    Float Es
    )
{
  rgb_spectrum rgb(0.0f);
  vector3f eye_dir((eye - surface_point).normalized());
  const vector3f light_dir((light.position - surface_point).normalized());
  const vector3f half((light_dir + eye_dir).normalized());
  rgb += Kd * std::max(0.0f, normal.dot(light_dir)) * light.rgb; // diffuse
  rgb += Ks * std::pow(std::max(0.0f, half.dot(normal)), Es) * light.rgb; // specular

  // assume uniform light
  return rgb * surface_rgb;
}

int main(int argc, char** argv) {

  //const vector2i img_res(3840, 2160); // 4k ftw
  const vector2i img_res(3840 * 2, 2160 * 2);
  //const vector2i img_res(1920, 1080);
  const vector2f ndc_res(1, 1);
  const vector3f eye_pos(0, 0, 5.2);

#ifndef ORTHO
  camera::persp cam(
      tf::look_at(vector3f(0), eye_pos, vector3f(0, 1, 0)),
      img_res,
      ndc_res,
      0.1,
      1000,
      math::radians(75),
      Float(img_res.x) / img_res.y
      );
#else
  camera::ortho cam(
      tf::look_at(vector3f(0), eye_pos, vector3f(0, 1, 0)),
      img_res,
      ndc_res,
      0,
      100
      );
#endif /* ORTHO */

  //shapes::de_sphere sphere(tf::transform(), 0.5f);
  shapes::de_inf_spheres sphere(matrix4f(1.0f), 0.8f, 5.2f);
  //shapes::de_mandelbulb sphere(tf::rotate({ 1.0f, 0.0f, 0.0f }, math::radians(90)).mat);

  std::unique_ptr<Imf::Rgba> ird_xyz(new Imf::Rgba[img_res.x * img_res.y]);

  const rgb_spectrum surface_rgb(1.0, 1.0, 1.0);

  std::vector<light_source> lights {
    //{ point3f(10, 10, 10),  rgb_spectrum(0.5f, 0, 0) },
    //{ point3f(-10, 10, 10), rgb_spectrum(0, 0.5f, 0) },
    //{ point3f(0, -10, 10),  rgb_spectrum(0, 0, 0.5f) },
    { point3f(0, 0, 2.6),  rgb_spectrum(0.7f) }
    //{ point3f(0, 0, 0.1), rgb_spectrum(0.7f, 0, 0) }
    //{ point3f(1, -2, 4), rgb_spectrum(0.7f, 0, 0) }
    //{ point3f(0, 0, 0.1), rgb_spectrum(1.0f, 1.0f, 1.0f) }
  };

  std::mutex mutex;
  std::mutex shadow_mutex;
  size_t n_rays_hit = 0;
  size_t n_shadow_rays_hit = 0;
  int n_threads = std::thread::hardware_concurrency();
  std::cout << "Detected " << n_threads << " logical cores" << std::endl;
  constexpr Float shadow_bias = 5e-4f;
  auto render_routine = [&](const vector2i& start, const vector2i& end) -> void {
    for (int y = start.y; y < end.y; ++y) {
      for (int x = start.x; x < end.x; ++x) {
        // only sample 1 ray for now (at the center of each pixel)
        const intersect_opts options;
        intersect_result result;

        ray r = cam.generate_ray(point2f(x + 0.5, y + 0.5));
        bool intersect = sphere.intersect(r, options, &result);
        int i = img_res.x * y + x;
        if (intersect) {
          // viewing ray hits surface
          const vector3f normal(result.normal);
          rgb_spectrum rgb(0.0f);

          // cast shadow rays
          intersect_result shadow_result;
          for (const light_source& light : lights) {
            const vector3f shadow_r_dir(light.position - result.hit_point);
            ray shadow_r(
                result.hit_point + shadow_bias * result.normal,
                shadow_r_dir,
                shadow_r_dir.size()
                );
            bool shadow_intersect = sphere.intersect(shadow_r, options, &shadow_result);
            if (shadow_intersect) {
              std::lock_guard<std::mutex> lock(shadow_mutex);
              ++n_shadow_rays_hit;
            } else {
              rgb += blinn_phong_brdf(
                  light,
                  result.hit_point,
                  surface_rgb,
                  normal,
                  eye_pos,
                  0.5,
                  1,
                  32
                  );
            }
          }

          ird_xyz.get()[i].r = rgb.r();
          ird_xyz.get()[i].g = rgb.g();
          ird_xyz.get()[i].b = rgb.b();
          {
            std::lock_guard<std::mutex> lock(mutex);
            ++n_rays_hit;
          }
        } else {
          ird_xyz.get()[i].r = 0;
          ird_xyz.get()[i].g = 0;
          ird_xyz.get()[i].b = 0;
        }
      }
    }
  };

  std::vector<std::thread> workers;
  const Float tile_width = img_res.x / n_threads;
  for (int x = 0; x < img_res.x; x += tile_width) {
    const vector2i start(x, 0);
    vector2i end(start.x + tile_width, img_res.y);
    if (start.x + tile_width >= img_res.x) {
      end.x = img_res.x;
    }
    workers.push_back(std::thread(render_routine, start, end));
  }

  std::cout << "Created " << workers.size() << " render workers" << std::endl;
  for (std::thread& worker : workers) {
    worker.join();
  }

  std::cout << n_rays_hit << " rays hit" << std::endl;
  std::cout << n_shadow_rays_hit << " shadow rays hit" << std::endl;

  Imf::RgbaOutputFile exr("output.exr", img_res.x, img_res.y, Imf::WRITE_RGB);
  exr.setFrameBuffer(ird_xyz.get(), 1, img_res.x);
  exr.writePixels(img_res.y);

  return 0;
}
