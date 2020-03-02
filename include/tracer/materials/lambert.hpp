#ifndef TRACER_MATERIALS_LAMBERT_HPP
#define TRACER_MATERIALS_LAMBERT_HPP

#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class lambert : public material {
      public:
        lambert(const sampled_spectrum& color, const sampled_spectrum& emittance)
          : material(color, sampled_spectrum(), emittance, REFLECT) {}

        sampled_spectrum bxdf(
            const vector3f& omega_in,
            const vector3f& omega_out,
            const normal3f& mf_normal,
            const light_transport& lt
            ) const override;

        light_transport sample(
            vector3f* omega_in,
            normal3f* mf_normal,
            Float* pdf,
            const vector3f& omega_out,
            const light_transport& lt,
            const point3f& u
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_LAMBERT_HPP */
