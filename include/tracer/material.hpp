#ifndef TRACER_MATERIAL_HPP
#define TRACER_MATERIAL_HPP

#include "spectrum.hpp"

namespace tracer {
  class material {
    public:
      const rgb_spectrum surface_rgb;
      const rgb_spectrum emittance;
      const Float Kd;
      const Float Ks;

      const enum transport_type {
        EMIT,
        REFLECT,
        TRANSMIT,
        SSS
      } transport;

      material(
          transport_type transport,
          const rgb_spectrum& rgb = rgb_spectrum(1),
          const rgb_spectrum& emittance = rgb_spectrum(0),
          Float Kd = 0.5,
          Float Ks = 0.5)
        : surface_rgb(rgb), emittance(emittance), Kd(Kd), Ks(Ks), transport(transport) {}

      /*
       * Evaluate weight term including w.n, BxDF, PDF and Lambert hack.
       * Multiply this function's result with incoming radiance can output outgoing radiance.
       */
      virtual rgb_spectrum weight(
          const vector3f& omega_in,
          const vector3f& omega_out,
          const normal3f& normal
          ) const = 0;
  };
}

#endif /* TRACER_MATERIAL_HPP */
