#include "tracer/materials/light.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    sampled_spectrum light::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& transport
        ) const
    {
      return sampled_spectrum();
    }

    light::light_transport light::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
        Float* pdf,
        const vector3f& omega_out,
        const light_transport& lt,
        const point3f& u
        ) const
    {
      return { EMIT, OUTSIDE };
    }
  }
}
