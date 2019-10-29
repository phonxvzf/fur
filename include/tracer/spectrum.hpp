#ifndef TRACER_SPECTRUM_HPP
#define TRACER_SPECTRUM_HPP

#include <vector>
#include "math/matrix.hpp"

namespace tracer {

  template <size_t N>
    class nspectrum {
      protected:
        Float spd[N]; // can be actual SPD or primary chromaticity
        static const size_t n_samples = N;

      public:
        nspectrum() {
          for (size_t i = 0; i < N; ++i) {
            spd[i] = 0;
          }
        }

        nspectrum(Float v) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] = v;
          }
        }

        nspectrum(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] = sp.spd[i];
          }
        }
        
        size_t get_n_samples() const { return n_samples; }

        bool is_black() const {
          for (size_t i = 0; i < n_samples; ++i) {
            if (!COMPARE_EQ(spd[i], 0)) return false;
          }
          return true;
        }

        nspectrum sqrt() const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = std::sqrt(spd[i]);
          }
          return result;
        }

        nspectrum pow(Float x) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = std::pow(spd[i], x);
          }
          return result;
        }

        nspectrum exp() const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = std::exp(spd[i]);
          }
          return result;
        }

        nspectrum clamp(Float min, Float max) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = math::clamp(spd[i], min, max);
          }
          return result;
        }

        nspectrum& operator=(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] = sp.spd[i];
          }
          return *this;
        }

        nspectrum operator+(const nspectrum& sp) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = spd[i] + sp.spd[i];
          }
          return result;
        }

        nspectrum operator-(const nspectrum& sp) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = spd[i] - sp.spd[i];
          }
          return result;
        }

        nspectrum operator*(const nspectrum& sp) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            result.spd[i] = spd[i] * sp.spd[i];
          }
          return result;
        }

        nspectrum operator/(const nspectrum& sp) const {
          nspectrum result;
          for (size_t i = 0; i < N; ++i) {
            assert(!COMPARE_EQ(sp.spd[i], 0));
            result.spd[i] = spd[i] / sp.spd[i];
          }
          return result;
        }

        nspectrum& operator+=(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] += sp.spd[i];
          }
          return *this;
        }

        nspectrum& operator-=(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] -= sp.spd[i];
          }
          return *this;
        }

        nspectrum& operator*=(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            spd[i] *= sp.spd[i];
          }
          return *this;
        }

        nspectrum& operator/=(const nspectrum& sp) {
          for (size_t i = 0; i < N; ++i) {
            assert(!COMPARE_EQ(sp.spd[i], 0));
            spd[i] /= sp.spd[i];
          }
          return *this;
        }

        Float& operator[](int i) { return spd[i]; }
        Float operator[](int i) const { return spd[i]; }
    };

  template <size_t N>
    inline nspectrum<N> lerp(Float t, const nspectrum<N>& sp_a, const nspectrum<N>& sp_b) {
      return (1 - t) * sp_a + t * sp_b;
    }

  template <size_t N>
    inline nspectrum<N> operator*(Float s, const nspectrum<N>& sp) {
      return sp * nspectrum<N>(s);
    }

  template <size_t N>
    inline nspectrum<N> operator*(const nspectrum<N>& sp, Float s) {
      return sp * nspectrum<N>(s);
    }

  template <size_t N>
    inline nspectrum<N> operator/(const nspectrum<N>& sp, Float s) {
      return sp / nspectrum<N>(s);
    }

  typedef struct { Float lambda; Float value; } spectral_sample;
  
  class rgb_spectrum : public nspectrum<3> {
    public:
      rgb_spectrum(Float x) : nspectrum<3>(x) {}
      rgb_spectrum(Float r, Float g, Float b) { spd[0] = r; spd[1] = g; spd[2] = b; }
      rgb_spectrum(const nspectrum<3>& sp) : nspectrum<3>(sp) {}
      rgb_spectrum(const rgb_spectrum& rgb) : nspectrum<3>(rgb) {}

      Float r() const { return spd[0]; }
      Float g() const { return spd[1]; }
      Float b() const { return spd[2]; }
  };

  class sampled_spectrum : public nspectrum<60> {
    public:
      static const int LAMBDA_START = 400;
      static const int LAMBDA_END = 700;
      static const int N_SPECTRAL_SAMPLES = 60;

      sampled_spectrum(Float v);
      sampled_spectrum(std::vector<spectral_sample> samples);

      math::vector3f xyz() const;

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

  inline math::vector3f xyz_to_rgb(const math::vector3f& xyz) {
    return math::matrix3f(
        { 3.240479f, -1.537150f, -0.498535f },
        { -0.969256f, 1.875991f, 0.041556f  },
        { 0.055648f, -0.204043f, 1.057311f  }
        ).dot(xyz);
  }

  inline math::vector3f rgb_to_xyz(const math::vector3f& rgb) {
    return math::matrix3f(
        { 0.4124533 , 0.35757984, 0.18042262 },
        { 0.21267127, 0.71515972, 0.07216883 },
        { 0.01933384, 0.11919363, 0.95022693 }
        ).dot(rgb);
  }

  extern const std::vector<Float> CIE_LAMBDAS;
  extern const std::vector<spectral_sample> CIE_X;
  extern const std::vector<spectral_sample> CIE_Y;
  extern const std::vector<spectral_sample> CIE_Z;

  extern const sampled_spectrum SMC_X;
  extern const sampled_spectrum SMC_Y;
  extern const sampled_spectrum SMC_Z;

} /* namespace tracer */

#endif /* TRACER_SPECTRUM_HPP */
