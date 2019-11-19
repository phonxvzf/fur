#ifndef TRACER_MATERIALS_LIGHT_HPP
#define TRACER_MATERIALS_LIGHT_HPP

#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class light : public material {
      public:
        light(const rgb_spectrum& emittance)
          : material(rgb_spectrum(0), rgb_spectrum(0), emittance, EMIT) {}

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

#endif /* TRACER_MATERIALS_LIGHT_HPP */
