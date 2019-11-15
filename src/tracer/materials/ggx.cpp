#include "tracer/materials/ggx.hpp"

namespace tracer {
  namespace materials {
    Float ggx::geometry(const normal3f& normal, const vector3f& omega, Float alpha2) const {
      const Float n_dot_omega = maxdot(normal, omega);
      return 2 * n_dot_omega
        / (n_dot_omega + std::sqrt(n_dot_omega * n_dot_omega * (1 - alpha2) + alpha2));
    }

    rgb_spectrum ggx::bxdf(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& normal
        ) const
    {
      const vector3f half = (omega_in + omega_out).normalized();

      const Float n_dot_half = maxdot(normal, half);

      Float denom = 1 + n_dot_half * n_dot_half * (alpha2 - 1);
      const Float ggx = alpha2 / (MATH_PI * denom * denom);

      // bi-directional geometry term
      const Float geom = 1 /
        (1 + geometry(normal, omega_out, alpha2) + geometry(normal, omega_in, alpha2));

      const rgb_spectrum F = fresnel + (rgb_spectrum(1) - fresnel)
        * std::pow(1 - std::max(Float(0), omega_in.dot(half)), 5);

      denom = std::max(Float(0), 4 * normal.dot(omega_out) * normal.dot(omega_in));
      if (COMPARE_EQ(denom, 0)) return rgb_spectrum(0);

      // Torrance-Sparrow
      return ggx * F * geom / denom;
    }
  }
}
