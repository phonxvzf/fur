#ifndef TRACER_MATERIAL_HPP
#define TRACER_MATERIAL_HPP

#include "spectrum.hpp"
#include "math/util.hpp"

namespace tracer {
  class material {
    public:
      const sampled_spectrum refl;
      const sampled_spectrum refr;
      const sampled_spectrum emittance;

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
          const sampled_spectrum& refl,
          const sampled_spectrum& refr,
          const sampled_spectrum& emittance,
          const transport_type& transport = REFLECT)
        : refl(refl), refr(refr), emittance(emittance), transport_model(transport) {}

      /*
       * Evaluate weight term including wi.n, BxDF, and PDF
       * Multiply this function's result with incoming radiance can output outgoing radiance.
       * All input vectors are in tangent space (up basis vector (macrosurface normal) is <0,1,0>)
       */
      virtual sampled_spectrum weight(
          const vector3f& omega_in,
          const vector3f& omega_out,
          const normal3f& mf_normal,
          const light_transport& lt
          ) const = 0;

      /*
       * Sample incoming ray direction omega_in based on the BxDF (importance sampling).
       * omega_out must be in tangent space. The returning vector will also be in tangent space.
       */
      virtual light_transport sample(
          vector3f* omega_in,
          normal3f* mf_normal,
          const vector3f& omega_out,
          const light_transport& lt,
          const point2f& u,
          Float e
          ) const = 0;

      inline bool is_refractive(transport_type tp) const {
        return tp == REFRACT || tp == SSS;
      }
  };
}

#endif /* TRACER_MATERIAL_HPP */
