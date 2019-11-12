#ifndef TRACER_MATERIAL_HPP
#define TRACER_MATERIAL_HPP

#include "spectrum.hpp"

namespace tracer {
  class material {
    public:
      rgb_spectrum surface_rgb;
      Float Kd;
      Float Ks;
        
      material(const rgb_spectrum& rgb = rgb_spectrum(1), Float Kd = 0.5, Float Ks = 0.5)
        : surface_rgb(rgb), Kd(Kd), Ks(Ks) {}
  };
}

#endif /* TRACER_MATERIAL_HPP */
