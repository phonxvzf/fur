#include "tracer/materials/lambert.hpp"

#include "math/sampler.hpp"

namespace tracer {
  namespace materials {
    sampled_spectrum lambert::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& lt
        ) const 
    {
      return refl; // this weight is evaluated by importance sampling (cosine-weighted)
    }

    material::light_transport lambert::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
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
