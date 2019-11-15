#include "tracer/materials/light.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum light::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& normal
        ) const
    {
      return rgb_spectrum(0);
    }
  }
}
