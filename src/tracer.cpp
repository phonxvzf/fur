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
      tf::look_at(vector3f(0), vector3f(0, 0, -10), vector3f(0, 1, 0)),
      img_res,
      -1,
      1
      );

  // uncomment this to cause undefined behaviour (so weird)
  // FIXME
  //tf::translate({ 0, 0, 0 });

  shapes::de_sphere sphere(matrix4f(1.0), 1.0);

  // Using memory from heap unknowningly causes undefined behaviour
  // (all rays do not intersect)
  // FIXME
  //std::unique_ptr<Imf::Rgba> ird_xyz(new Imf::Rgba[300 * 300]);
  //Imf::Rgba *ird_xyz = (Imf::Rgba*) malloc(sizeof(Imf::Rgba) * img_res.x * img_res.y);
  Imf::Rgba ird_xyz[500 * 500];

  const intersect_opts options;
  intersect_result result;

  for (int x = 0; x < img_res.x; ++x) {
    for (int y = 0; y < img_res.y; ++y) {
      // only sample 1 ray for now (at the center of each pixel)
      ray r = cam.generate_ray(point2f(x + 0.5, y + 0.5));
      bool intersect = sphere.intersect(r, options, &result);
      int i = img_res.x * y + x;
      if (intersect) {
        ird_xyz[i].r = 1;
        ird_xyz[i].g = 1;
        ird_xyz[i].b = 1;
        ird_xyz[i].a = 1;
      } else {
        ird_xyz[i].r = 0;
        ird_xyz[i].g = 0;
        ird_xyz[i].b = 0;
        ird_xyz[i].a = 1;
      }
    }
  }

  Imf::RgbaOutputFile exr("output.exr", img_res.x, img_res.y);
  exr.setFrameBuffer(ird_xyz, 1, img_res.x);
  exr.writePixels(img_res.y);

  return 0;
}
