#include "tracer/materials/dipole.hpp"
#include "math/util.hpp"

namespace tracer {
  namespace materials {
    dipole::dipole(
        const sampled_spectrum& refl,
        const sampled_spectrum& emittance,
        const sampled_spectrum& sigma_s,
        Float beta_n,
        Float beta_m,
        Float eta_i,
        Float eta_t,
        Float k
        )
      : material(refl, refl, emittance, HAIR),
      sigma_s(sigma_s), beta_n(beta_n), beta_m(beta_m), eta_i(eta_i), eta_t(eta_t), k(k)
    {
      // map reflectance to sigma_a
      // use the same mapping in path traced version
      for (size_t i = 0; i < sigma_a.get_n_samples(); ++i) {
        sigma_a[i] = pow2(std::log(std::max(refl[i], FLOAT_TOLERANT))
          / (5.969f - 0.215f * beta_n + 2.532f * pow2(beta_n)
              - 10.73f * pow3(beta_n) + 5.574f * pow4(beta_n) + 0.245f * pow5(beta_n)));
      }
      sigma_t = sigma_a + sigma_s;
      albedo = sigma_s / sigma_t;
      sigma_tr = (3.f * sigma_a * sigma_t).sqrt();
    }

    sampled_spectrum dipole::Rd(Float r) const {
      // here z is y
      if (COMPARE_EQ(r, 0)) return 0;
      Float Fdr = fresnel_diffuse(eta_t / eta_i);
      Float A = (1 + Fdr) / (1 - Fdr);
      sampled_spectrum zr = sigma_t.inverse();
      sampled_spectrum zv = -(1 + 4 * A / 3) * zr;
      sampled_spectrum r2(pow2(r));
      sampled_spectrum dr = (r2 + zr.pow(2)).sqrt();
      sampled_spectrum dv = (r2 + zv.pow(2)).sqrt();

      sampled_spectrum first_term, second_term;
      sampled_spectrum one(1.f);
      sampled_spectrum dr3 = dr.pow(3);
      sampled_spectrum dv3 = dv.pow(3);
      if (dr3.has_zero()) return 0;
      first_term = zr * (one + sigma_tr * dr) * (-1.f * sigma_tr * dr).exp() / dr3;
      second_term = zv * (one + sigma_tr * dv) * (-1.f * sigma_tr * dv).exp() / dv3;
      return INV_FOUR_PI * albedo * (first_term - second_term);
    }

    sampled_spectrum dipole::Sd(Float ft_in, Float ft_out, const sampled_spectrum& rd) const {
      return (INV_PI * ft_in * ft_out) * rd;
    }

    sampled_spectrum dipole::S1(const vector3f& omega_in, const vector3f& omega_out) const {
      vector3f h = (omega_in + omega_out).normalized();
      Float n = math::lerp(beta_n, 0, 32);
      return INV_EIGHT_PI * std::pow(std::abs(h.y), n);
    }

    sampled_spectrum dipole::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& lt
        ) const
    {
      // assume very near omega_in and omega_out
      // use same normal vector <0,1,0>
      if (COMPARE_EQ(omega_in.y, 0)) return 0.f;
      normal3f normal(0, 1, 0);
      vector3f refl_in = reflect(omega_in, normal);
      vector3f refl_out = reflect(omega_out, normal);
      Float ft_in = 1.f - fresnel(refl_in, normal, eta_i, eta_t);
      Float ft_out = 1.f - fresnel(refl_out, normal, eta_i, eta_t);
      Float r = mf_normal.size();

      return k * (Sd(ft_in, ft_out, Rd(r)) + (1.f - ft_in) * S1(omega_in, omega_out))
        / std::abs(omega_in.y);
    }

    material::light_transport dipole::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
        Float* pdf,
        const vector3f& omega_out,
        const light_transport& lt,
        const point3f& u
        ) const
    {
      *omega_in = sampler::sample_cosine_hemisphere(point2f(u));

      // FIXME: importance sample Rd
      mf_normal->x = standard_normal.sample(u[0]);
      mf_normal->y = 0.f;
      mf_normal->z = standard_normal.sample(u[1]);

      normal3f normal(0, 1, 0);
      vector3f refl_in = reflect(*omega_in, normal);
      Float ft_in = 1.f - fresnel(refl_in, normal, eta_i, eta_t);

      *omega_in = (*omega_in + *mf_normal).normalized();
      *pdf = Rd(mf_normal->size()).luminance()
        + (1.f - ft_in) * S1(*omega_in, omega_out).luminance();
      return { REFLECT, OUTSIDE };
    }
  }
}
