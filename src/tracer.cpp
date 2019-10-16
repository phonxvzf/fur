#include <iostream>
#include <memory>
#include <OpenEXR/ImfRgbaFile.h>

#include "math/transform.hpp"
#include "tracer/camera.hpp"
#include "tracer/spectrum.hpp"
#include "tracer/shapes/de_sphere.hpp"

int main(int argc, char** argv) {
  using namespace tracer;
  using namespace math;

  const vector2i img_res(500, 500);

  camera::ortho cam(
      tf::look_at(vector3f(0), vector3f(1, 0, 1), vector3f(0, 1, 0)),
      img_res,
      0,
      2
      );

  shapes::de_sphere sphere(matrix4f(1.0), 1.0);

  std::unique_ptr<Imf::Rgba> ird_xyz(new Imf::Rgba[img_res.x * img_res.y]);

  const intersect_opts options;
  intersect_result result;

  const point3f light_pos(10, 0, 10);
  const vector3f light_rgb(1.0, 0.0, 0.0);
  const vector3f surface_rgb(1.0, 1.0, 1.0);

  int count = 0;
  for (int x = 0; x < img_res.x; ++x) {
    for (int y = 0; y < img_res.y; ++y) {
      // only sample 1 ray for now (at the center of each pixel)
      ray r = cam.generate_ray(point2f(x + 0.5, y + 0.5));
      bool intersect = sphere.intersect(r, options, &result);
      int i = img_res.x * y + x;
      if (intersect) {
        const vector3f normal(result.normal.normalized());
        const vector3f light_dir((light_pos - result.hit_point).normalized());
        const vector3f rgb(normal.dot(light_dir) * surface_rgb * light_rgb);
        ird_xyz.get()[i].r = rgb.x;
        ird_xyz.get()[i].g = rgb.y;
        ird_xyz.get()[i].b = rgb.z;
        ++count;
      } else {
        ird_xyz.get()[i].r = 0;
        ird_xyz.get()[i].g = 0;
        ird_xyz.get()[i].b = 0;
      }
    }
  }

  std::cout << count << " rays hit" << std::endl;

  Imf::RgbaOutputFile exr("output.exr", img_res.x, img_res.y, Imf::WRITE_RGB);
  exr.setFrameBuffer(ird_xyz.get(), 1, img_res.x);
  exr.writePixels(img_res.y);

  return 0;
}
