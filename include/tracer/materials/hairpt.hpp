#ifndef TRACER_MATERALS_HAIRPT_HPP
#define TRACER_MATERALS_HAIRPT_HPP

#include "tracer/material.hpp"
#include "math/pdf.hpp"

namespace tracer {
  namespace materials {

    extern const Float sqrt_pi_over_eight;

    class hairpt : public material {
      private:
        const Float eta_i, eta_t;
        const Float beta_m;
        const Float beta_n;
        const Float alpha;
        Float v[4];
        Float logistic_s;
        sampled_spectrum sigma_a;

        // Compute longitudinal scattering distribution function
        Float lsdf(
            Float sin_theta_in,
            Float sin_theta_out,
            Float cos_theta_in,
            Float cos_theta_out,
            Float v
            ) const;

        Float gaussian_detector(
            int lobe,
            Float phi,
            Float gamma_o,
            Float gamma_t,
            Float s
            ) const;

        sampled_spectrum transmittance(
            Float sin_theta_out,
            Float cos_theta_out,
            Float h,
            Float* sin_gamma_o,
            Float* sin_gamma_t
            ) const;

        void attenuation(sampled_spectrum a[4], Float f, const sampled_spectrum& tr) const;
        void attenuation_prob(Float prob[4], const sampled_spectrum a[4]) const;

        Float specular_cone_angle(Float theta, int lobe) const;

        inline Float net_deflection(int lobe, Float gamma_o, Float gamma_t) const {
          return 2.f * lobe * gamma_t - 2.f * gamma_o + lobe * PI;
        }

      public:
        hairpt(
            const sampled_spectrum& refl,
            const sampled_spectrum& emittance,
            Float eta_i,
            Float eta_t,
            Float beta_m,
            Float beta_n,
            Float alpha
            );

        sampled_spectrum weight(
            const vector3f& omega_in,
            const vector3f& omega_out,
            const normal3f& mf_normal,
            const light_transport& lt
            ) const override;

        light_transport sample(
            vector3f* omega_in,
            normal3f* mf_normal,
            const vector3f& omega_out,
            const light_transport& lt,
            const point2f& u,
            Float xi
            ) const override;
    };
  }
}

#endif /* TRACER_MATERALS_HAIRPT_HPP */
