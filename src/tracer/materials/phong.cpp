#include "tracer/materials/phong.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum phong::weight(vector3f omega_in, vector3f omega_out) const {
      const vector3f half = (omega_in + omega_out).normalized();
      const Float dot = maxdot(omega_in, half);
      return (Kd * surface_rgb * INV_PI + rgb_spectrum(std::pow(maxdot(half, { 0, 1, 0 }), Es)))
        * dot;
    }

    vector3f phong::sample(
        const vector3f omega_out,
        const point2f& u
        ) const
    {
      // TODO
      return vector3f(0);
    }
  }
}
