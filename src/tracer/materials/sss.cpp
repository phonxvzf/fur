#include "tracer/materials/sss.hpp"

namespace tracer {
  namespace materials {
    sss::sss(const rgb_spectrum& rgb_refl,
        const rgb_spectrum& rgb_refr,
        const rgb_spectrum& emittance,
        Float roughness,
        Float eta_i,
        Float eta_t,
        const rgb_spectrum& sigma_a,
        const rgb_spectrum& sigma_s,
        Float g)
      : ggx(rgb_refl, rgb_refr, emittance, roughness, eta_i, eta_t, transport_type::SSS),
      sigma_a(sigma_a), sigma_s(sigma_s), sigma(sigma_a + sigma_s), inv_sigma(sigma.inverse()),
      g(g), absorp_prob((sigma_a * inv_sigma).average()) {}

    sss::sss(const sss& cpy)
      : ggx(cpy), sigma_a(cpy.sigma_a), sigma_s(cpy.sigma_s),
      sigma(cpy.sigma), inv_sigma(cpy.inv_sigma), g(cpy.g), absorp_prob(cpy.absorp_prob) {}

    Float sss::sample_distance(random::rng& rng) const {
      const int channel = rng.next_ui() % 3;
      return -std::log(1 - rng.next_uf()) * inv_sigma[channel];
    }

    rgb_spectrum sss::transmittance(Float dist) const {
      return (-1.f * sigma * std::min(dist, std::numeric_limits<Float>::max())).exp();
    }

    rgb_spectrum sss::beta(bool inside, Float dist) const {
      rgb_spectrum tr = transmittance(dist);
      rgb_spectrum density = inside ? sigma * tr : tr;
      Float p = pdf(density);
      if (COMPARE_EQ(p, 0)) return rgb_spectrum(1);
      return inside ? tr * sigma_s / p : tr / p;
    }

    Float sss::pdf(const rgb_spectrum& tr) const {
      return tr.average();
    }
  }
}
