#include "tracer/materials/phong.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum phong::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& normal
        ) const
    {
      const vector3f& half = (omega_in + omega_out).normalized();
      return rgb_spectrum(std::pow(maxdot(half, normal), Es));
    }
  }
}
