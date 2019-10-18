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

using namespace tracer;
using namespace math;

struct light_source {
  const vector3f position;
  const vector3f rgb;
};

// Blinn-phong
vector3f blinn_phong_brdf(
    const std::vector<light_source>& light_sources,
    const point3f& surface_point,
    const vector3f& surface_rgb,
    const normal3f& normal,
    const point3f& eye,
    Float Kd,
    Float Ks,
    Float Es
    )
{
  vector3f rgb(0.0f);
  vector3f eye_dir((eye - surface_point).normalized());
  for (const light_source& light : light_sources) {
    const vector3f light_dir((light.position - surface_point).normalized());
    const vector3f diffuse(Kd * std::max(0.0f, normal.dot(light_dir)) * light.rgb);
    const vector3f half((light_dir + eye_dir).normalized());
    const vector3f specular(Ks * std::pow(std::max(0.0f, half.dot(normal)), Es) * light.rgb);
    rgb += diffuse + specular;
  }
  return rgb * surface_rgb;
}

int main(int argc, char** argv) {

  const vector2i img_res(1920, 1080);
  const vector2f ndc_res(1, 1);
  const vector3f eye_pos(0, 0, 10);

#ifndef ORTHO
  camera::persp cam(
      tf::look_at(vector3f(0), eye_pos, vector3f(0, 1, 0)),
      img_res,
      ndc_res,
      0.1,
      100,
      math::radians(60),
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

  // TODO: shadows

  std::unique_ptr<Imf::Rgba> ird_xyz(new Imf::Rgba[img_res.x * img_res.y]);

  const vector3f surface_rgb(1.0, 1.0, 1.0);

  std::vector<light_source> lights {
    { vector3f(10, 10, 10),  vector3f(0.5f, 0, 0)  },
    { vector3f(-10, 10, 10), vector3f(0, 0.5f, 0)  },
    { vector3f(0, -10, 10),  vector3f(0, 0, 0.5f)  }
  };

  std::mutex mutex;
  int n_rays_hit = 0;
  int n_threads = std::thread::hardware_concurrency();
  std::cout << "Detected " << n_threads << " logical cores" << std::endl;
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
          // ray hits surface
          const vector3f normal(result.normal.normalized());
          const vector3f rgb(blinn_phong_brdf(
                lights,
                result.hit_point,
                surface_rgb,
                normal,
                eye_pos,
                0.5,
                1,
                32
                )
              );
          ird_xyz.get()[i].r = rgb.x;
          ird_xyz.get()[i].g = rgb.y;
          ird_xyz.get()[i].b = rgb.z;
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

  for (std::thread& worker : workers) {
    worker.join();
  }

  std::cout << n_rays_hit << " rays hit" << std::endl;

  Imf::RgbaOutputFile exr("output.exr", img_res.x, img_res.y, Imf::WRITE_RGB);
  exr.setFrameBuffer(ird_xyz.get(), 1, img_res.x);
  exr.writePixels(img_res.y);

  return 0;
}
