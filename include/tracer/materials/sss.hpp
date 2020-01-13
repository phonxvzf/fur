#ifndef TRACER_MATERIALS_SSS_HPP
#define TRACER_MATERIALS_SSS_HPP

#include "tracer/materials/ggx.hpp"
#include "math/random.hpp"

namespace tracer {
  namespace materials {
    class sss : public ggx {
      private:
        const sampled_spectrum sigma_a;
        const sampled_spectrum sigma_s;
        const sampled_spectrum sigma, inv_sigma;

      public:
        const Float g;
        const Float absorp_prob;

        sss(const sampled_spectrum& refl,
            const sampled_spectrum& refr,
            const sampled_spectrum& emittance,
            Float roughness,
            Float eta_i,
            Float eta_t,
            const sampled_spectrum& sigma_a,
            const sampled_spectrum& sigma_s,
            Float g = 0);

        sss(const sss& cpy);

        sampled_spectrum transmittance(Float dist) const;
        sampled_spectrum beta(bool inside, Float dist) const;

        Float sample_distance(random::rng& rng) const;
        Float pdf(const sampled_spectrum& tr) const;
    };
  }
}

#endif /* TRACER_MATERIALS_SSS_HPP */
