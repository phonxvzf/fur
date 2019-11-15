#ifndef TRACER_MATERIALS_GGX_HPP
#define TRACER_MATERIALS_GGX_HPP

#include "tracer/material.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    class ggx : public material {
      private:
        const Float alpha2;
        Float geometry(const normal3f& normal, const vector3f& omega, Float alpha2) const;

      public:
        const rgb_spectrum fresnel;

        ggx(
            transport_type transport,
            const rgb_spectrum& emittance,
            const rgb_spectrum& fresnel,
            const rgb_spectrum& diffuse,
            Float roughness,
            Float Kd = 0)
          : material(transport, diffuse, emittance, Kd),
          alpha2(pow4(roughness)),
          fresnel(fresnel) {}

        ggx(const ggx& cpy)
          : material(cpy.transport, cpy.surface_rgb, cpy.emittance, cpy.Kd), alpha2(cpy.alpha2) {}
      
        rgb_spectrum weight(
            const vector3f& omega_in,
            const vector3f& mf_normal,
            const normal3f& normal
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_GGX_HPP */
