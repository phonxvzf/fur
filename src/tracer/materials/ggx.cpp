#include "tracer/materials/ggx.hpp"
#include "math/sampler.hpp"

namespace tracer {
  namespace materials {
    Float ggx::geometry(
        const normal3f& normal,
        const normal3f& mf_normal,
        const vector3f& omega
        ) const
    {
      Float dot = omega.dot(normal);
      Float dotm = omega.dot(mf_normal);
      if (COMPARE_LEQ(dot * dotm, 0)) return 0; // sidedness agreement
      Float cos2 = pow2(dot);
      if (COMPARE_EQ(cos2, 0)) return 0;
      Float tan2 = 1 / cos2 - 1;
      return chi_plus(dotm / dot) * 2 / (1 + std::sqrt(1 + alpha2 * tan2));
    }

    Float ggx::distribution(const normal3f& normal, const normal3f& mf_normal) const {
      if (COMPARE_EQ(normal.dot(mf_normal), 0)) return 0;
      Float sec2 = 1 / pow2(normal.dot(mf_normal));
      Float tan2 = sec2 - 1;
      return alpha2 * chi_plus(mf_normal.dot(normal)) * INV_PI * pow2(sec2) / pow2(alpha2 + tan2);
    }

    sampled_spectrum ggx::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& lt
        ) const
    {
      if (COMPARE_EQ(std::abs(omega_in.y), 0)) return sampled_spectrum(0.f);
      if ((omega_in + omega_out).is_zero()) return sampled_spectrum(1.f);

      const normal3f normal(0, 1, 0);

      // Smith geometry term G = G1_in * G1_out
      const Float G = geometry(normal, mf_normal, omega_in)
        * geometry(normal, mf_normal, omega_out);

      const Float inv_sample_weight = absdot(omega_in, normal) * absdot(mf_normal, normal)
        / (absdot(omega_in, mf_normal) * G);

      return clamp(inv_sample_weight, Float(0), Float(1))
        * (lt.transport == REFLECT ? refl : refr) / absdot(omega_in, { 0, 1, 0 });
    }

    material::light_transport ggx::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
        Float* pdf,
        const vector3f& omega_out,
        const light_transport& lt,
        const point3f& u
        ) const
    {
      const Float theta = std::atan2(alpha * std::sqrt(u.x) , std::sqrt(1 - u.x));
      const Float phi = TWO_PI * u.y;
      const normal3f m = spherical_coords(theta, phi);

      *mf_normal = m;
      *pdf = 1.f; // we already know sample weight, hacking this gives nice result

      if (is_refractive(transport_model)) {
        Float eta_i_chk = eta_i, eta_t_chk = eta_t;
        if (lt.med == OUTSIDE) std::swap(eta_i_chk, eta_t_chk);

        // check for external & internal reflection
        const vector3f refl = reflect(omega_out, m);
        if (u.z < fresnel(refl, m, eta_t_chk, eta_i_chk)) {
          *omega_in = refl;
          return { lt.med == INSIDE ? REFRACT : REFLECT, lt.med };
        }

        bool tir = false;
        *omega_in = refract(omega_out, m, eta_t_chk / eta_i_chk, &tir);
        if (tir) return { REFRACT, lt.med };
        return { REFRACT, medium(lt.med ^ 1) };
      }

      *omega_in = reflect(omega_out, m);
      if (omega_in->y < 0) *omega_in = sampler::sample_cosine_hemisphere(point2f(u));
      return { REFLECT, OUTSIDE };
    }
  }
}
