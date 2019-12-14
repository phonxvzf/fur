#include "tracer/materials/ggx.hpp"

namespace tracer {
  namespace materials {

    Float ggx::geometry(
        const normal3f& normal,
        const normal3f& mf_normal,
        const vector3f& omega,
        Float alpha2) const
    {
      Float dot = omega.dot(normal);
      Float dotm = omega.dot(mf_normal);
      if (COMPARE_LEQ(dot * dotm, 0)) return 0;
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

    rgb_spectrum ggx::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const light_transport& lt
        ) const
    {
      if ((omega_in + omega_out).is_zero()) return rgb_spectrum(lt.transport == REFRACT);

      const normal3f normal = (lt.transport == REFLECT) ? normal3f(0, 1, 0) : normal3f(0, -1, 0);
      const normal3f mf_normal = (lt.transport == REFLECT) ? (omega_in + omega_out).normalized()
        : (lt.med == OUTSIDE) ? (eta_i * omega_in + eta_t * omega_out).normalized()
        : (eta_t * omega_in + eta_i * omega_out).normalized();

      const Float n_dot_m   = absdot(normal, mf_normal);
      const Float in_dot_n  = absdot(omega_out, normal);
      const Float denom = n_dot_m * in_dot_n;
      if (COMPARE_EQ(denom, 0)) return rgb_spectrum(0);

      // Smith geometry term G = G1_in * G1_out
      const Float G = geometry(normal, mf_normal, omega_in, alpha2)
        * geometry(normal, mf_normal, omega_out, alpha2);

      return (absdot(omega_in, mf_normal) * G / denom)
        * (lt.transport == REFLECT ? rgb_refl : rgb_refr);
    }

    ggx::light_transport ggx::sample(
        vector3f* omega_in,
        const vector3f& omega_out,
        const light_transport& lt,
        const point2f& u,
        Float xi) const
    {
      const Float cos_theta = std::sqrt((1 - u.x) / (u.x * (alpha2 - 1) + 1));
      const Float sin_theta = sin_from_cos(cos_theta);
      const Float phi = TWO_PI * u.y;
      const Float cos_phi = std::cos(phi);
      const Float sin_phi = sin_from_cos_theta(cos_phi, phi);
      const vector3f m(sin_theta * cos_phi, cos_theta, sin_theta * sin_phi);

      if (transport_model == REFRACT) {
        Float eta_i_chk = eta_i, eta_t_chk = eta_t;
        if (lt.med == OUTSIDE) std::swap(eta_i_chk, eta_t_chk);

        // check for external & internal reflection
        const vector3f refl = reflect(omega_out, m);
        if (xi < fresnel(refl, m, eta_t_chk, eta_i_chk)) {
          *omega_in = refl;
          return { REFLECT, lt.med };
        }

        *omega_in = COMPARE_EQ(eta_i_chk, eta_t_chk) ?
          -omega_out : refract(omega_out, { 0, 1, 0 }, m, eta_t_chk / eta_i_chk);
        return { REFRACT, medium(lt.med ^ 1) };
      }

      *omega_in = reflect(omega_out, m);
      return { REFLECT, OUTSIDE };
    }
  }
}
