#ifndef TRACER_MATERIALS_PHONG_HPP
#define TRACER_MATERIALS_PHONG_HPP

#include "tracer/material.hpp"
#include "tracer/spectrum.hpp"

namespace tracer {
  namespace materials {
    class phong : public material {
      public:
        const Float Es;

        phong(const rgb_spectrum& rgb, Float Kd, Float Ks, Float Es)
          : material(REFLECT, rgb, Kd, Ks), Es(Es) {}
      
        rgb_spectrum bxdf(
          const vector3f& omega_in,
          const vector3f& omega_out,
          const normal3f& normal
          ) const override;
    };
  }
}

#endif /* TRACER_MATERIALS_PHONG_HPP */
