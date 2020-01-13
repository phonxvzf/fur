#ifndef TRACER_MATERIALS_LIGHT_HPP
#define TRACER_MATERIALS_LIGHT_HPP

#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class light : public material {
      public:
        light(const sampled_spectrum& emittance)
          : material(sampled_spectrum(), sampled_spectrum(), emittance, EMIT) {}

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

#endif /* TRACER_MATERIALS_LIGHT_HPP */
