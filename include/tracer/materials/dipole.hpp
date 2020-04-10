#include "tracer/material.hpp"
#include "math/sampler.hpp"

namespace tracer {
  namespace materials {
    class dipole : public material {
      private:
        sampled_spectrum sigma_a;
        sampled_spectrum sigma_s;
        sampled_spectrum sigma_t;
        sampled_spectrum sigma_tr;
        sampled_spectrum albedo;

        Float beta_n;
        Float beta_m;
        Float eta_i;
        Float eta_t;
        Float k;

        sampler::normal1d standard_normal;

        sampled_spectrum Sd(Float ft_in, Float ft_out, const sampled_spectrum& rd) const;
        sampled_spectrum Rd(Float r) const;
        sampled_spectrum S1(const vector3f& omega_in, const vector3f& omega_out) const;

      public:
        dipole(
            const sampled_spectrum& refl,
            const sampled_spectrum& emittance,
            const sampled_spectrum& sigma_s,
            Float beta_n,
            Float beta_m,
            Float eta_i,
            Float eta_t,
            Float k = 1
            );

        sampled_spectrum bxdf(
            const vector3f& omega_in,
            const vector3f& omega_out,
            const normal3f& mf_normal,
            const light_transport& lt
            ) const override;

        light_transport sample(
            vector3f* omega_in,
            normal3f* mf_normal,
            Float* pdf,
            const vector3f& omega_out,
            const light_transport& lt,
            const point3f& u
            ) const override;
    };
  }
}
