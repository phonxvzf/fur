#ifndef TRACER_SPECTRUM_HPP
#define TRACER_SPECTRUM_HPP

#include <vector>
#include "math/matrix.hpp"

namespace tracer {
  
  using namespace math;

  class nspectrum {
    protected:
      std::vector<Float> spd; // can be real SPD or primary chromaticity (e.g. RGB, XYZ)
      const size_t n_samples;

    public:
      explicit nspectrum(size_t N, Float v = 0) : n_samples(N) {
        spd.resize(N, v);
      }

      nspectrum(const nspectrum& sp) : spd(sp.spd), n_samples(sp.n_samples) {}

      nspectrum& operator=(const nspectrum& sp);

      size_t get_n_samples() const { return n_samples; }

      bool is_black() const;
      nspectrum sqrt() const;
      nspectrum pow(Float x) const;
      nspectrum exp() const;
      nspectrum clamp(Float min, Float max) const;
      nspectrum inverse() const;
      Float average() const;

      nspectrum operator+(const nspectrum& sp) const;
      nspectrum operator-(const nspectrum& sp) const;
      nspectrum operator*(const nspectrum& sp) const;
      nspectrum operator/(const nspectrum& sp) const;

      nspectrum operator*(Float s) const;
      nspectrum operator/(Float s) const;

      nspectrum& operator+=(const nspectrum& sp);
      nspectrum& operator-=(const nspectrum& sp);
      nspectrum& operator*=(const nspectrum& sp);
      nspectrum& operator/=(const nspectrum& sp);

      Float& operator[](int i) { return spd[i]; }
      Float operator[](int i) const { return spd[i]; }
  };

  inline nspectrum operator*(Float s, const nspectrum& sp) {
    return sp * s;
  }

  inline nspectrum lerp(Float t, const nspectrum& sp_a, const nspectrum& sp_b) {
    return (1 - t) * sp_a + t * sp_b;
  }


  typedef struct { Float lambda; Float value; } spectral_sample;

  class rgb_spectrum : public nspectrum {
    public:
      explicit rgb_spectrum(Float x = 0) : nspectrum(3, x) {}
      rgb_spectrum(Float r, Float g, Float b) : nspectrum(3) {
        spd[0] = r;
        spd[1] = g;
        spd[2] = b;
      }
      rgb_spectrum(const nspectrum& sp) : nspectrum(sp) {}
      rgb_spectrum(const rgb_spectrum& rgb) : nspectrum(rgb) {}

      Float r() const { return spd[0]; }
      Float g() const { return spd[1]; }
      Float b() const { return spd[2]; }

      inline Float max() const {
        return std::max(spd[0], std::max(spd[1], spd[2]));
      }

      friend std::ostream& operator<<(std::ostream& os, const rgb_spectrum& rgb) {
        return os << "rgb(" << rgb.spd[0] << ", " << rgb.spd[1] << ", " << rgb.spd[2] << ")";
      }
  };

  class sampled_spectrum : public nspectrum {
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
