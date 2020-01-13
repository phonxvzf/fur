#ifndef TRACER_MATERIALS_GGX_HPP
#define TRACER_MATERIALS_GGX_HPP

#include "tracer/material.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    class ggx : public material {
      private:
        const Float alpha;
        const Float alpha2;
        const Float eta_i;
        const Float eta_t;

        Float distribution(const normal3f& normal, const normal3f& mf_normal) const;

        Float geometry(
            const normal3f& normal,
            const normal3f& mf_normal,
            const vector3f& omega,
            Float alpha2
            ) const;

      public:
        ggx(const sampled_spectrum& refl,
            const sampled_spectrum& refr,
            const sampled_spectrum& emittance,
            Float roughness,
            Float eta_i,
            Float eta_t,
            const transport_type& transport = REFLECT)
          : material(refl, refr, emittance, transport),
          alpha(pow2(roughness)),
          alpha2(pow2(alpha)),
          eta_i(eta_i),
          eta_t(eta_t) {}

        ggx(const ggx& cpy)
          : material(cpy.refl, cpy.refr, cpy.emittance),
          alpha(cpy.alpha),
          alpha2(cpy.alpha2),
          eta_i(cpy.eta_i),
          eta_t(cpy.eta_t) {}

        sampled_spectrum weight(
            const vector3f& omega_in,
            const vector3f& omega_out,
            const light_transport& lt
            ) const override;

        light_transport sample(
            vector3f* omega_in,
            const vector3f& omega_out,
            const light_transport& lt,
            const point2f& u,
            Float xi
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_GGX_HPP */
