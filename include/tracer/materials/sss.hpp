#ifndef TRACER_MATERIALS_SSS_HPP
#define TRACER_MATERIALS_SSS_HPP

#include "tracer/materials/ggx.hpp"
#include "math/random.hpp"

namespace tracer {
  namespace materials {
    class sss : public ggx {
      private:
        const rgb_spectrum sigma_a;
        const rgb_spectrum sigma_s;
        const rgb_spectrum sigma, inv_sigma;

      public:
        const Float g;
        const Float absorp_prob;

        sss(const rgb_spectrum& rgb_refl,
            const rgb_spectrum& rgb_refr,
            const rgb_spectrum& emittance,
            Float roughness,
            Float eta_i,
            Float eta_t,
            const rgb_spectrum& sigma_a,
            const rgb_spectrum& sigma_s,
            Float g = 0);

        sss(const sss& cpy);

        Float sample_distance(random::rng& rng) const;
        rgb_spectrum trasmittance(Float dist) const;
        Float pdf(const rgb_spectrum& tr) const;
    };
  }
}

#endif /* TRACER_MATERIALS_SSS_HPP */
