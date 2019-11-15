#ifndef TRACER_MATERIAL_HPP
#define TRACER_MATERIAL_HPP

#include "spectrum.hpp"

namespace tracer {
  class material {
    public:
      const rgb_spectrum surface_rgb;
      const Float Kd;
      const Float Ks;

      const enum transport_type {
        REFLECT,
        TRANSMIT,
        SSS
      } transport;

      material(
          transport_type transport,
          const rgb_spectrum& rgb = rgb_spectrum(1),
          Float Kd = 0.5,
          Float Ks = 0.5)
        : surface_rgb(rgb), Kd(Kd), Ks(Ks), transport(transport) {}

      /*
       * Evaluate BxDF term. All vectors must be already normalized.
       */
      virtual rgb_spectrum bxdf(
          const vector3f& omega_in,
          const vector3f& omega_out,
          const normal3f& normal
          ) const = 0;
  };
}

#endif /* TRACER_MATERIAL_HPP */
