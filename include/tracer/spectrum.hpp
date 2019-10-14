#ifndef TRACER_SPECTRUM_HPP
#define TRACER_SPECTRUM_HPP

#include <vector>
#include "math/float.hpp"

namespace tracer {

  template <size_t N>
    class nspectrum {
      protected:
        Float spd[N];
        const size_t n_samples;

      public:
        nspectrum();
        nspectrum(Float v);
        nspectrum(const nspectrum& sp);

        nspectrum& operator=(const nspectrum& sp);

        bool is_black() const;
        size_t get_n_samples() const { return n_samples; }

        nspectrum sqrt() const;
        nspectrum pow(Float x) const;
        nspectrum exp() const;
        nspectrum clamp(Float min, Float max) const;

        nspectrum operator+(const nspectrum& sp) const;
        nspectrum operator-(const nspectrum& sp) const;
        nspectrum operator*(const nspectrum& sp) const;
        nspectrum operator/(const nspectrum& sp) const;

        nspectrum& operator+=(const nspectrum& sp);
        nspectrum& operator-=(const nspectrum& sp);
        nspectrum& operator*=(const nspectrum& sp);
        nspectrum& operator/=(const nspectrum& sp);

        Float& operator[](int i) { return spd[i]; }
        Float operator[](int i) const { return spd[i]; }
    };

  template <size_t N>
    inline nspectrum<N> lerp(Float t, const nspectrum<N>& sp_a, const nspectrum<N>& sp_b) {
      return (1 - t) * sp_a + t * sp_b;
    }

  typedef struct { Float lambda; Float value; } spectral_sample;

  class sampled_spectrum : public nspectrum<60> {
    public:
      static const int LAMBDA_START = 400;
      static const int LAMBDA_END = 700;
      static const int N_SPECTRAL_SAMPLES = 60;

      sampled_spectrum(Float v);
      sampled_spectrum(std::vector<spectral_sample> samples);

      static Float average_spectral_samples(
          const std::vector<spectral_sample>& samples,
          const Float lambda0,
          const Float lambda1
          );
      static std::vector<spectral_sample> create_spectral_samples(
          const std::vector<Float>& lambdas,
          const std::vector<Float>& values
          );
  };

  extern const std::vector<Float> CIE_LAMBDAS;
  extern const std::vector<spectral_sample> CIE_X;
  extern const std::vector<spectral_sample> CIE_Y;
  extern const std::vector<spectral_sample> CIE_Z;

  extern const sampled_spectrum X_SPECTRUM;
  extern const sampled_spectrum Y_SPECTRUM;
  extern const sampled_spectrum Z_SPECTRUM;

} /* namespace tracer */

#endif /* TRACER_SPECTRUM_HPP */
