#include "tracer/materials/lambert.hpp"

#include "math/sampler.hpp"

namespace tracer {
  namespace materials {
    rgb_spectrum lambert::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const light_transport& lt
        ) const 
    {
      const normal3f normal = (omega_in + omega_out).normalized();
      return rgb_refl * INV_PI * absdot(omega_in, normal);
    }

    material::light_transport lambert::sample(
        vector3f* omega_in,
        const vector3f& omega_out,
        const light_transport& lt,
        const point2f& u,
        Float e
        ) const
    {
      *omega_in = sampler::sample_cosine_hemisphere(u);
      return { REFLECT, OUTSIDE };
    }
  }
}
