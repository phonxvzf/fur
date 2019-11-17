#include "tracer/materials/ggx.hpp"

namespace tracer {
  namespace materials {

    Float ggx::geometry(
        const normal3f& normal,
        const normal3f& mf_normal,
        const vector3f& omega,
        Float alpha2) const
    {
      Float cos2 = pow2(omega.dot(normal));
      if (COMPARE_EQ(cos2, 0)) return 0;
      Float tan2 = 1 / cos2 - 1;
      Float dot = omega.dot(normal);
      if (COMPARE_EQ(dot, 0)) return 0;
      return chi_plus(omega.dot(mf_normal) / dot) * 2 / (1 + std::sqrt(1 + alpha2 * tan2));
    }
 
    Float ggx::distribution(const normal3f& normal, const normal3f& mf_normal) const {
      const Float cos2 = pow2(normal.dot(mf_normal));
      if (COMPARE_EQ(cos2, 0)) return 0;
      return alpha2 * INV_PI / pow2(cos2 * (alpha2 - 1) + 1);
    }

    rgb_spectrum ggx::weight(
        vector3f omega_in,
        vector3f omega_out
        ) const
    {
      const normal3f normal(0, 1, 0);
      const vector3f mf_normal = (omega_in + omega_out).normalized();

      const Float n_dot_m = maxdot(normal, mf_normal);
      const Float in_dot_n = omega_in.dot(normal);
      const Float in_dot_m = omega_in.dot(mf_normal);
      const Float out_dot_n = maxdot(omega_out, normal);
      if (COMPARE_EQ(n_dot_m, 0)
          || COMPARE_EQ(in_dot_n, 0)
          || COMPARE_EQ(in_dot_m, 0)
          || COMPARE_EQ(out_dot_n, 0)) return rgb_spectrum(0);

      // Smith geometry term G = G1_in * G1_out
      const Float G = geometry(normal, mf_normal, omega_in, alpha2)
        * geometry(normal, mf_normal, omega_out, alpha2);

      return (in_dot_m * G / (in_dot_n * n_dot_m)) * fresnel;
    }

    vector3f ggx::sample(vector3f omega_out, const point2f& u) const {
      const Float cos_theta = std::sqrt((1 - u.x) / (u.x * (alpha2 - 1) + 1));
      const Float sin_theta = sin_from_cos(cos_theta);
      const Float phi = TWO_PI * u.y;
      const Float cos_phi = std::cos(phi);
      const Float sin_phi = sin_from_cos_theta(cos_phi, phi);
      const vector3f m(sin_theta * cos_phi, cos_theta, sin_theta * sin_phi);
      return reflect(omega_out, m);
    }
  }
}
