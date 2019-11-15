#include "tracer/materials/phong.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum phong::weight(
        const vector3f& omega_in,
        const vector3f& half,
        const normal3f& normal
        ) const
    {
      const Float dot = maxdot(omega_in, normal);
      return (Kd * surface_rgb * INV_PI + rgb_spectrum(std::pow(maxdot(half, normal), Es))) * dot;
    }
  }
}
