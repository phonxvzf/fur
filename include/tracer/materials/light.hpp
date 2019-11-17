#ifndef TRACER_MATERIALS_LIGHT_HPP
#define TRACER_MATERIALS_LIGHT_HPP
#include "tracer/material.hpp"

namespace tracer {
  namespace materials {
    class light : public material {
      public:
        light(const rgb_spectrum& rgb)
          : material(EMIT, rgb_spectrum(0), rgb) {}

        rgb_spectrum weight(
            vector3f omega_in,
            vector3f omega_out
            ) const override;

        vector3f sample(
            const vector3f omega_out,
            const point2f& u
            ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_LIGHT_HPP */
