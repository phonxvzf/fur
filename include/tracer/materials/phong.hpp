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

#endif /* TRACER_MATERIALS_PHONG_HPP */
