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

        Float geometry(
            const normal3f& normal,
            const normal3f& mf_normal,
            const vector3f& omega,
            Float alpha2
            ) const;

        Float distribution(const normal3f& normal, const normal3f& mf_normal) const;

      public:
        const rgb_spectrum fresnel;

        ggx(
            transport_type transport,
            const rgb_spectrum& emittance,
            const rgb_spectrum& fresnel,
            Float roughness,
            Float Kd = 0)
          : material(transport, fresnel, emittance, Kd),
          alpha(pow2(roughness)),
          alpha2(pow2(alpha)),
          fresnel(fresnel) {}

        ggx(const ggx& cpy)
          : material(cpy.transport, cpy.surface_rgb, cpy.emittance, cpy.Kd),
          alpha(cpy.alpha),
          alpha2(cpy.alpha2) {}

        rgb_spectrum weight(
            vector3f omega_in,
            vector3f omega_out
            ) const override;

        vector3f sample(
            vector3f omega_out,
            const point2f& u
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_GGX_HPP */
