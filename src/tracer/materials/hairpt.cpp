#include "tracer/materials/hairpt.hpp"
#include "math/sampler.hpp"
#include <cmath>

namespace tracer {
  namespace materials {

    const Float sqrt_pi_over_eight = 0.626657068f;

    hairpt::hairpt(
        const sampled_spectrum& refl,
        const sampled_spectrum& emittance,
        Float eta_i,
        Float eta_t,
        Float beta_m,
        Float beta_n,
        Float alpha)
      : material(
          sampled_spectrum(1.f), // hair color is only dictated by sigma_a
          sampled_spectrum(1.f),
          emittance,
          HAIR
          ), eta_i(eta_i), eta_t(eta_t), beta_m(beta_m), beta_n(beta_n), alpha(alpha)
    {
      // calculate longitudinal scattering variance
      const Float v0 = pow2(0.726f * beta_m + 0.812f * pow2(beta_m) + 3.7f * pow20(beta_m));
      v[0] = v0;
      v[1] = 0.25f * v0;
      v[2] = 4.f * v0;
      v[3] = v[2];

      // map reflectance to sigma_a
      for (size_t i = 0; i < sigma_a.get_n_samples(); ++i) {
        sigma_a[i] = pow2(std::log(std::max(refl[i], FLOAT_TOLERANT))
          / (5.969f - 0.215f * beta_n + 2.532f * pow2(beta_n)
              - 10.73f * pow3(beta_n) + 5.574f * pow4(beta_n) + 0.245f * pow5(beta_n)));
      }
      sigma_a = sigma_a.clamp(1e-5f, std::numeric_limits<Float>::infinity());

      // map azimuthal roughness to logistic scale factor s
      logistic_s = 0.265f * beta_n + 1.194f * pow2(beta_n) + 5.372f * pow20(beta_n) * pow2(beta_n);
      logistic_s = sqrt_pi_over_eight * logistic_s;
    }

    sampled_spectrum hairpt::transmittance(
        Float sin_theta_out,
        Float cos_theta_out,
        Float h,
        Float* sin_gamma_o,
        Float* sin_gamma_t
        ) const
    {
      Float sin_theta_t = sin_theta_out / eta_t;
      Float cos_theta_t = cos_from_sin(sin_theta_t);

      Float modified_eta = std::sqrt(pow2(eta_t) - pow2(sin_theta_out)) / cos_theta_out;
      *sin_gamma_o = h;
      *sin_gamma_t = h / modified_eta;

      Float dist = 2.f * cos_from_sin(*sin_gamma_t) / cos_theta_t;
      return (-dist * sigma_a).exp();
    }

    Float hairpt::lsdf(
        Float sin_theta_in,
        Float sin_theta_out,
        Float cos_theta_in,
        Float cos_theta_out,
        Float v
        ) const
    {
      if (COMPARE_EQ(v, 0)) return 1;
      Float inv_v = 1.f / v;
      Float sin_term = sin_theta_in * sin_theta_out * inv_v;
      Float cos_term = cos_theta_in * cos_theta_out * inv_v;
      Float I0 = std::cyl_bessel_i(0.f, cos_term);
      Float logI0 = cos_term > 12.f ?
        cos_term + 0.5f * (-std::log(TWO_PI) + std::log(1 / cos_term) + 1 / (8 * cos_term))
        : std::log(I0);
      static constexpr Float ln2 = 0.6931471805f;
      return v < 0.1f ?
        std::exp(logI0 - sin_term - inv_v + ln2 + std::log(0.5f * inv_v))
        : (0.5f * inv_v / std::sinh(inv_v)) * std::exp(-sin_term) * I0;
    }

    Float hairpt::gaussian_detector(
        int lobe,
        Float phi,
        Float gamma_o,
        Float gamma_t,
        Float s
        ) const
    {
      Float dphi = phi - net_deflection(lobe, gamma_o, gamma_t);
      while (dphi < -PI) dphi += TWO_PI;
      while (dphi > PI) dphi -= TWO_PI;
      return logistic_pdf_finite_norm(s, dphi, -PI, PI);
    }

    void hairpt::attenuation(sampled_spectrum a[4], Float f, const sampled_spectrum& tr) const {
      // R
      a[0] = sampled_spectrum(f);
      // TT
      a[1] = pow2(1.f - f) * tr;
      // TRT
      a[2] = pow2(1.f - f) * f * tr * tr;
      // TRRT and onto infinity
      sampled_spectrum denom = sampled_spectrum(1.f) - f * tr;
      a[3] = pow2(1.f - f) * pow2(f) * tr * tr * tr / denom;
    }

    void hairpt::attenuation_prob(Float prob[4], const sampled_spectrum a[4]) const {
      Float sum = 0.f;
      for (int i = 0; i < 4; ++i) {
        prob[i] = a[i].luminance();
        sum += prob[i];
      }
      for (int i = 0; i < 4; ++i) {
        prob[i] = prob[i] / sum;
      }
    }

    Float hairpt::specular_cone_angle(Float theta, int lobe) const {
      switch (lobe) {
        case 0:
          return -theta + 2.f * alpha;
        case 1:
          return -theta - alpha;
        case 2:
          return -theta - 4.f * alpha;
      }
      return -theta;
    }

    sampled_spectrum hairpt::weight(
        const vector3f& omega_in,
        const vector3f& omega_out,
        const normal3f& uvw,
        const light_transport& lt
        ) const
    {
      Float sin_theta_out = omega_out.x;
      Float cos_theta_out = cos_from_sin(sin_theta_out);
      Float phi_out = std::atan2(omega_out.y, omega_out.z);

      Float sin_theta_in = omega_in.x;
      Float cos_theta_in = cos_from_sin(sin_theta_in);
      Float phi_in = std::atan2(omega_in.y, omega_in.z);

      Float phi = phi_in - phi_out;

      // longitudinal scattering for each lobe
      Float M[4];
      for (int i = 0; i < 4; ++i) {
        M[i] = lsdf(sin_theta_in, sin_theta_out, cos_theta_in, cos_theta_out, v[i]);
      }

      // attenuation for each lobe
      Float h = 2.f * uvw[1] - 1; // offset from surface to central medulla in range [-1,1]
      Float sin_gamma_o, sin_gamma_t;
      sampled_spectrum T = transmittance(
          sin_theta_out,
          cos_theta_out,
          h,
          &sin_gamma_o,
          &sin_gamma_t
          );
      Float cos_gamma_o = cos_from_sin(sin_gamma_o);
      sampled_spectrum A[4];
      Float f = fresnel_cosine(cos_theta_out * cos_gamma_o, eta_t, eta_i);
      attenuation(A, f, T);
      Float A_prob[4];
      attenuation_prob(A_prob, A);

      Float D[4];
      Float gamma_o = asin_clamp(sin_gamma_o);
      Float gamma_t = asin_clamp(sin_gamma_t);
      for (int i = 0; i < 4; ++i) {
        D[i] = gaussian_detector(i, phi, gamma_o, gamma_t, logistic_s);
      }

      sampled_spectrum bcsdf(0.f);
      for (int i = 0; i < 4; ++i) {
        bcsdf += M[i] * D[i] * A[i]; // TODO: hair cuticle (tilt by alpha)
      }

      Float pdf = 0.f;
      for (int i = 0; i < 4; ++i) {
        pdf += M[i] * A_prob[i] * D[i];
      }

      if (COMPARE_EQ(pdf, 0)) return sampled_spectrum(1.f);

      return bcsdf / pdf;
    } /* weight() */

    material::light_transport hairpt::sample(
        vector3f* omega_in,
        normal3f* mf_normal, // this value is exceptionally valid (bad practice, though)
        const vector3f& omega_out,
        const light_transport& lt,
        const point2f& u,
        Float xi
        ) const
    {
      point2f u_demux[2] = { demux_float(u[0]), demux_float(u[1]) };
      const vector3f uvw = *mf_normal;
      Float sin_theta_out = omega_out.x;
      Float cos_theta_out = cos_from_sin(sin_theta_out);
      Float theta_out = asin_clamp(sin_theta_out);
      Float phi_out = std::atan2(omega_out.y, omega_out.z);

      Float h = 2.f * uvw[1] - 1; // offset from surface to central medulla in range [-1,1]
      Float sin_gamma_o, sin_gamma_t;
      sampled_spectrum T = transmittance(
          sin_theta_out,
          cos_theta_out,
          h,
          &sin_gamma_o,
          &sin_gamma_t
          );
      Float cos_gamma_o = cos_from_sin(sin_gamma_o);
      sampled_spectrum A[4];
      Float f = fresnel_cosine(cos_theta_out * cos_gamma_o, eta_t, eta_i);
      attenuation(A, f, T);
      Float A_prob[4];
      attenuation_prob(A_prob, A);
      Float gamma_o = asin_clamp(sin_gamma_o);
      Float gamma_t = asin_clamp(sin_gamma_t);

      // determine lobe, next ray offset will be determined by omega_out.dot(omega_in)
      int lobe = 0;
      for (; lobe < 4; ++lobe) {
        if (u_demux[0][0] < A_prob[lobe]) break;
        u_demux[0][0] -= A_prob[lobe];
      }

      // sample longitudinal angle according to Mp (LSDF)
      // taken from d'Eon's paper
      Float modified_theta = PI_OVER_TWO - specular_cone_angle(theta_out, lobe);
      Float inv_v = 1.f / v[lobe];
      Float uxi = v[lobe] < 0.3f ? // exp may reach inf when variance is too low
        1.f : v[lobe] * std::log(std::exp(inv_v) - 2.f * u_demux[0][1] * std::sinh(inv_v));
      const Float sine = uxi * std::cos(modified_theta)
          + std::sqrt(1 - pow2(uxi)) * std::cos(TWO_PI * u_demux[1][0]) * std::sin(modified_theta);
      const Float theta_in = asin_clamp(sine);
      const Float cos_theta_in = std::cos(theta_in);
      const Float sin_theta_in = sine;

      // sample azimuthal according to Np (ASDF)
      Float dphi = TWO_PI * u_demux[1][1];
      if (lobe < 3) dphi = net_deflection(lobe, gamma_o, gamma_t)
        + sampler::sample_finite_norm_logistic(
            logistic_s,
            u_demux[1][1],
            -PI,
            PI);
      const Float phi_in = phi_out + dphi;
      const Float cos_phi_in = std::cos(phi_in);
      const Float sin_phi_in = std::sin(phi_in);

      *omega_in = right_to_left(vector3f(
            sin_theta_in, cos_theta_in * cos_phi_in, cos_theta_in * sin_phi_in
            ));

      // FIXME
      //if (lobe < 3) {
      //  fprintf(stderr, "lobe %d: yo = %f yi = %f\n", lobe, omega_out.y, omega_in->y);
      //  ASSERT((sign(omega_in->y) != sign(omega_out.y)) == lobe % 2);
      //}
      return { REFLECT, OUTSIDE };
    }
  } /* namespace materials */
} /* namespace tracer */