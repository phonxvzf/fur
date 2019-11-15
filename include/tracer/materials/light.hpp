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
          const vector3f& omega_in,
          const vector3f& omega_out,
          const normal3f& normal
          ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_LIGHT_HPP */
