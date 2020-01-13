#include "tracer/materials/light.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    sampled_spectrum light::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const light_transport& transport
        ) const
    {
      return sampled_spectrum();
    }

    light::light_transport light::sample(
        vector3f* omega_in,
        const vector3f& omega_out,
        const light_transport& lt,
        const point2f& u,
        Float e
        ) const
    {
      return { EMIT, OUTSIDE };
    }
  }
}
