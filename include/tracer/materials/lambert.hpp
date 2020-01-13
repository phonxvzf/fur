#ifndef TRACER_MATERIALS_LAMBERT_HPP
#define TRACER_MATERIALS_LAMBERT_HPP

#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class lambert : public material {
      public:
        lambert(const sampled_spectrum& color, const sampled_spectrum& emittance)
          : material(color, sampled_spectrum(), emittance, REFLECT) {}

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
            Float e
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_LAMBERT_HPP */
