#ifndef TRACER_MATERIAL_HPP
#define TRACER_MATERIAL_HPP

#include "spectrum.hpp"
#include "math/util.hpp"

namespace tracer {
  class material {
    public:
      const rgb_spectrum rgb_refl;
      const rgb_spectrum rgb_refr;
      const rgb_spectrum emittance;

      enum transport_type {
        REFLECT,
        REFRACT,
        SSS,
        EMIT,
        NONE
      } transport_model;

      typedef uint8_t medium;

      struct light_transport {
        transport_type transport;
        medium med;
      };

      material(
          const rgb_spectrum& rgb_refl = rgb_spectrum(1),
          const rgb_spectrum& rgb_refr = rgb_spectrum(1),
          const rgb_spectrum& emittance = rgb_spectrum(0),
          const transport_type& transport = REFLECT)
        : rgb_refl(rgb_refl),
        rgb_refr(rgb_refr),
        emittance(emittance),
        transport_model(transport) {}

      /*
       * Evaluate weight term including w.n, BxDF, and PDF
       * Multiply this function's result with incoming radiance can output outgoing radiance.
       * All input vectors are in tangent space (up basis vector (surface normal) is <0,1,0>)
       */
      virtual rgb_spectrum weight(
          const vector3f& omega_in,
          const vector3f& omega_out,
          const light_transport& lt
          ) const = 0;

      /*
       * Sample incoming ray direction omega_in based on the BxDF (importance sampling).
       * omega_out must be in tangent space. The returning vector will also be in tangent space.
       */
      virtual light_transport sample(
          vector3f* omega_in,
          const vector3f& omega_out,
          const light_transport& lt,
          const point2f& u,
          Float e
          ) const = 0;
  };
}

#endif /* TRACER_MATERIAL_HPP */
