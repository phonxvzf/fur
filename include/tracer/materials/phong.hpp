#ifndef TRACER_MATERIALS_PHONG_HPP
#define TRACER_MATERIALS_PHONG_HPP

#include "tracer/material.hpp"
#include "tracer/spectrum.hpp"

namespace tracer {
  namespace materials {
    class phong : public material {
      public:
        rgb_spectrum surface_rgb = rgb_spectrum(1.0f);
        Float Kd = 0.5f;
        Float Ks = 0.8f;
        Float Es = 32;

        phong() {}
        phong(const rgb_spectrum& rgb, Float Kd, Float Ks, Float Es)
          : surface_rgb(rgb), Kd(Kd), Ks(Ks), Es(Es) {}
    };
  }
}

#endif /* TRACER_MATERIALS_PHONG_HPP */
