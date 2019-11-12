#ifndef TRACER_MATERIALS_PHONG_HPP
#define TRACER_MATERIALS_PHONG_HPP

#include "tracer/material.hpp"
#include "tracer/spectrum.hpp"

namespace tracer {
  namespace materials {
    class phong : public material {
      public:
        Float Es;

        phong() : material(), Es(32) {}
        phong(const rgb_spectrum& rgb, Float Kd, Float Ks, Float Es)
          : material(rgb, Kd, Ks), Es(Es) {}
    };
  }
}

#endif /* TRACER_MATERIALS_PHONG_HPP */
