#include "tracer/materials/light.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum light::weight(
        const vector3f omega_in,
        const vector3f omega_out
        ) const
    {
      return rgb_spectrum(0);
    }

    vector3f light::sample(
        const vector3f omega_out,
        const point2f& u
        ) const
    {
      return vector3f(0);
    }
  }
}
