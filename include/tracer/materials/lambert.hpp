#ifndef TRACER_MATERIALS_LAMBERT_HPP
#define TRACER_MATERIALS_LAMBERT_HPP

#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class lambert : public material {
      public:
        lambert(const rgb_spectrum& color, const rgb_spectrum& emittance)
          : material(color, rgb_spectrum(0), emittance, REFLECT) {}

        rgb_spectrum weight(
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
