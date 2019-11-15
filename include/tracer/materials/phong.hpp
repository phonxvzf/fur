#ifndef TRACER_MATERIALS_PHONG_HPP
#define TRACER_MATERIALS_PHONG_HPP

#include "tracer/material.hpp"
#include "tracer/spectrum.hpp"

namespace tracer {
  namespace materials {
    class phong : public material {
      public:
        const Float Es;

        phong(const rgb_spectrum& rgb, const rgb_spectrum& emittance, Float Kd, Float Ks, Float Es)
          : material(REFLECT, rgb, emittance, Kd, Ks), Es(Es) {}
      
        rgb_spectrum weight(
          const vector3f& omega_in,
          const vector3f& half,
          const normal3f& normal
          ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_PHONG_HPP */
