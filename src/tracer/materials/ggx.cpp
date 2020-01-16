#include "tracer/materials/ggx.hpp"

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
      const Float cos2 = pow2(normal.dot(mf_normal));
      if (COMPARE_EQ(cos2, 0)) return 0;
      return alpha2 * INV_PI / pow2(cos2 * (alpha2 - 1) + 1);
    }

    sampled_spectrum ggx::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& mf_normal,
        const light_transport& lt
        ) const
    {
      if ((omega_in + omega_out).is_zero()) return sampled_spectrum(is_refractive(lt.transport));

      const normal3f normal(0, 1, 0);

      // Smith geometry term G = G1_in * G1_out
      const Float G = geometry(normal, mf_normal, omega_in)
        * geometry(normal, mf_normal, omega_out);

      const Float prob = absdot(omega_in, normal) * absdot(mf_normal, normal);

      return clamp((absdot(omega_in, mf_normal) * G / prob), Float(0), Float(1))
        * (lt.transport == REFLECT ? refl : refr);
    }

    material::light_transport ggx::sample(
        vector3f* omega_in,
        normal3f* mf_normal,
        const vector3f& omega_out,
        const light_transport& lt,
        const point2f& u,
        Float xi
        ) const
    {
      const Float theta = std::atan(alpha * std::sqrt(u.x / (1 - u.x)));
      const Float phi = TWO_PI * u.y;
      const Float sin_theta = std::sin(theta);
      const normal3f m(right_to_left(
            { sin_theta * std::cos(phi), sin_theta * std::sin(phi), std::cos(theta) })
          );

      *mf_normal = m;

      if (is_refractive(transport_model)) {
        Float eta_i_chk = eta_i, eta_t_chk = eta_t;
        if (lt.med == OUTSIDE) std::swap(eta_i_chk, eta_t_chk);

        // check for external & internal reflection
        const vector3f refl = reflect(omega_out, m);
        if (xi < fresnel(refl, m, eta_t_chk, eta_i_chk)) {
          *omega_in = refl;
          return { lt.med == INSIDE ? REFRACT : REFLECT, lt.med };
        }

        bool tir = false;
        *omega_in = refract(omega_out, m, eta_t_chk / eta_i_chk, &tir);
        if (tir) return { REFRACT, lt.med };
        return { REFRACT, medium(lt.med ^ 1) };
      }

      *omega_in = reflect(omega_out, m);
      return { REFLECT, OUTSIDE };
    }
  }
}
