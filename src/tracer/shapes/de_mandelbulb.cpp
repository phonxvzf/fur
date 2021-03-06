#include "tracer/shapes/de_mandelbulb.hpp"

namespace tracer {
  namespace shapes {
    de_mandelbulb::de_mandelbulb(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface)
      : destimator(shape_to_world, surface) {}

    de_mandelbulb::de_mandelbulb(const de_mandelbulb& cpy)
      : destimator(cpy.tf_shape_to_world, cpy.surface) {}

    Float de_mandelbulb::distance_function(const point3f& pt) const {
      // code taken from the Stanford's cs348b course
      using namespace std;
      const float bailout = 2.0f;
      const float Power = 8.0f;
      point3f z = pt;
      float dr = 1.0;
      float r = 0.0;
      for (int i = 0; i < 1000; i++) {
        r = (z - point3f(0,0,0)).size();
        if (r > bailout) break;

        // convert to polar coordinates
        float theta = acos(z.z / r);
        float phi = atan2(z.y, z.x);
        dr = pow(r, Power - 1.0) * Power * dr + 1.0;

        // scale and rotate the point
        float zr = pow(r, Power);
        theta = theta * Power;
        phi = phi * Power;

        // convert back to cartesian coordinates
        z = zr * point3f(sin(theta) * cos(phi), sin(phi) * sin(theta), cos(theta));
        z += pt;
      }
      return 0.5 * log(r) * r / dr;
    }

    bounds3f de_mandelbulb::bounds() const {
      // TODO
      return bounds3f(point3f(0));
    }
  }
}
