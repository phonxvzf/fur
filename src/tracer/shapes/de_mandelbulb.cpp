#include "tracer/shapes/de_mandelbulb.hpp"

namespace tracer {
  namespace shapes {
    de_mandelbulb::de_mandelbulb(const matrix4f& shape_to_world)
      : destimator(shape_to_world) {}

    de_mandelbulb::de_mandelbulb(const de_mandelbulb& cpy)
      : destimator(cpy.tf_shape_to_world) {}

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

      // mandelbox
      /*
      point3f z = pt;
      for (int iter = 0; iter < 1000; ++iter) {
        for (int i = 0; i < 3; ++i) {
          Float x = z[i];
          if (x > 1) {
            z[i] = 2 - z[i];
          } else if (x < -1) {
            z[i] = -2 - z[i];
          }
        }
        Float sz_sq = z.size_sq();
        if (sz_sq < 0.25) {
          z *= 4;
        } else if (sz_sq < 1) {
          z /= sz_sq;
        }
        z = 2 * z;
      }
      return sqrt(z.dot(z));
      */
    }
  }
}
