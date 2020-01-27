#include "tracer/materials/sss.hpp"

namespace tracer {
  namespace materials {
    sss::sss(const sampled_spectrum& refl,
        const sampled_spectrum& refr,
        const sampled_spectrum& emittance,
        Float roughness,
        Float eta_i,
        Float eta_t,
        const sampled_spectrum& sigma_a,
        const sampled_spectrum& sigma_s,
        Float g)
      : ggx(refl, refr, emittance, roughness, eta_i, eta_t, transport_type::SSS),
      sigma_a(sigma_a), sigma_s(sigma_s), sigma(sigma_a + sigma_s), inv_sigma(sigma.inverse()),
      g(g), absorp_prob((sigma_a * inv_sigma).average()) {}

    sss::sss(const sss& cpy)
      : ggx(cpy), sigma_a(cpy.sigma_a), sigma_s(cpy.sigma_s),
      sigma(cpy.sigma), inv_sigma(cpy.inv_sigma), g(cpy.g), absorp_prob(cpy.absorp_prob) {}

    Float sss::sample_distance(random::rng& rng) const {
      const int channel = rng.next_ui() % inv_sigma.get_n_samples();
      return -std::log(1 - rng.next_uf()) * inv_sigma[channel];
    }

    sampled_spectrum sss::transmittance(Float dist) const {
      return (-1.f * sigma * std::min(dist, std::numeric_limits<Float>::max())).exp();
    }

    sampled_spectrum sss::density(const sampled_spectrum& tr, bool inside) const {
      return inside ? static_cast<sampled_spectrum>(sigma * tr) : tr;
    }

    sampled_spectrum sss::beta(const sampled_spectrum& tr, bool inside) const {
      return inside ? static_cast<sampled_spectrum>(tr * sigma_s) : tr;
    }

    Float sss::pdf(const sampled_spectrum& density_spec) const {
      return density_spec.average();
    }
  }
}
