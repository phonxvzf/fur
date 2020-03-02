#include "tracer/materials/lambert.hpp"
#include "math/sampler.hpp"

namespace tracer {
  namespace materials {
    sampled_spectrum lambert::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& lt
        ) const 
    {
      return refl;
    }

    material::light_transport lambert::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
        Float* pdf,
        const vector3f& omega_out,
        const light_transport& lt,
        const point3f& u
        ) const
    {
      *omega_in = sampler::sample_cosine_hemisphere(point2f(u));
      *pdf = std::abs(omega_in->y);
      return { REFLECT, OUTSIDE };
    }
  }
}
