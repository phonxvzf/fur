#include "tracer/materials/ggx.hpp"

namespace tracer {
  namespace materials {
    Float ggx::geometry(const normal3f& normal, const vector3f& omega, Float alpha2) const {
      const Float n_dot_omega = maxdot(normal, omega);
      return 2 * n_dot_omega
        / (n_dot_omega + std::sqrt(n_dot_omega * n_dot_omega * (1 - alpha2) + alpha2));
    }

    rgb_spectrum ggx::weight(
        const vector3f& omega_in,
        const vector3f& mf_normal,
        const normal3f& normal
        ) const
    {
      const Float n_dot_m = maxdot(normal, mf_normal);
      const vector3f omega_out = reflect(omega_in, mf_normal);

      Float denom = 1 + n_dot_m * n_dot_m * (alpha2 - 1);
      const Float ggx = alpha2 / (MATH_PI * denom * denom);

      // bi-directional geometry term
      const Float geom = 1 /
        (1 + geometry(normal, omega_out, alpha2) + geometry(normal, omega_in, alpha2));

      const rgb_spectrum F = fresnel + (rgb_spectrum(1) - fresnel)
        * std::pow(1 - std::max(Float(0), omega_in.dot(mf_normal)), 5);

      denom = std::max(Float(0), 4 * normal.dot(omega_out) * normal.dot(omega_in));
      if (COMPARE_EQ(denom, 0)) return rgb_spectrum(0);

      const rgb_spectrum diffuse_hack = Kd * surface_rgb * INV_PI;
      const rgb_spectrum w = (diffuse_hack + ggx * F * geom / denom) * maxdot(omega_in, normal);

      denom = 4 * maxdot(omega_in, mf_normal);
      const rgb_spectrum pdf = COMPARE_EQ(denom, 0) ? rgb_spectrum(1) : n_dot_m / denom * F;

      return rgb_spectrum(
          COMPARE_EQ(w.r(), 0) ? 0 : w.r() / pdf.r(),
          COMPARE_EQ(w.g(), 0) ? 0 : w.g() / pdf.g(),
          COMPARE_EQ(w.b(), 0) ? 0 : w.b() / pdf.b()
          );
    }
  }
}
