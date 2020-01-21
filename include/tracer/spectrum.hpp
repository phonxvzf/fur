#ifndef TRACER_SPECTRUM_HPP
#define TRACER_SPECTRUM_HPP

#include <vector>
#include <memory>
#include <functional>
#include "math/matrix.hpp"

namespace tracer {

  using namespace math;

  class nspectrum {
    protected:
      // can be real SPD or chromaticity coeficients (e.g. RGB, XYZ)
      std::unique_ptr<Float[]> spd;
      const size_t n_samples;

    public:
      explicit nspectrum(size_t N = 0, Float v = 6) : n_samples(N) {
        spd = std::make_unique<Float[]>(N);
        std::fill_n(spd.get(), N, v);
      }

      nspectrum(const nspectrum& sp) : n_samples(sp.n_samples) {
        spd = std::make_unique<Float[]>(sp.n_samples);
        std::copy_n(sp.spd.get(), n_samples, spd.get());
      }

      nspectrum& operator=(const nspectrum& sp);

      size_t get_n_samples() const { return n_samples; }

      bool is_black() const;
      nspectrum sqrt() const;
      nspectrum pow(Float x) const;
      nspectrum exp() const;
      nspectrum clamp(Float min, Float max) const;
      nspectrum inverse() const;
      Float average() const;
      Float max() const;

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

      Float& operator[](int i) { return spd.get()[i]; }
      Float operator[](int i) const { return spd.get()[i]; }

      static const enum spectrum_type {
        NONE, RGB, XYZ, SAMPLED
      } type = NONE;
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
        spd.get()[0] = r;
        spd.get()[1] = g;
        spd.get()[2] = b;
      }
      rgb_spectrum(const nspectrum& sp) : nspectrum(sp) {}
      rgb_spectrum(const rgb_spectrum& rgb) : nspectrum(rgb) {}

      Float r() const { return spd.get()[0]; }
      Float g() const { return spd.get()[1]; }
      Float b() const { return spd.get()[2]; }

      friend std::ostream& operator<<(std::ostream& os, const rgb_spectrum& rgb) {
        return os
          << "rgb("
          << rgb.spd.get()[0] << ", "
          << rgb.spd.get()[1] << ", "
          << rgb.spd.get()[2] << ")";
      }

      static const spectrum_type type = RGB;
  };

  class xyz_spectrum : public nspectrum {
    public:
      explicit xyz_spectrum(Float x = 0) : nspectrum(3, x) {}
      xyz_spectrum(Float x, Float y, Float z) : nspectrum(3) {
        spd.get()[0] = x;
        spd.get()[1] = y;
        spd.get()[2] = z;
      }
      xyz_spectrum(const nspectrum& sp) : nspectrum(sp) {}
      xyz_spectrum(const xyz_spectrum& xyz) : nspectrum(xyz) {}

      Float x() const { return spd.get()[0]; }
      Float y() const { return spd.get()[1]; }
      Float z() const { return spd.get()[2]; }

      friend std::ostream& operator<<(std::ostream& os, const xyz_spectrum& xyz) {
        return os
          << "xyz("
          << xyz.spd.get()[0] << ", "
          << xyz.spd.get()[1] << ", "
          << xyz.spd.get()[2] << ")";
      }

      static const spectrum_type type = XYZ;
  };

  inline rgb_spectrum xyz_to_rgb(const xyz_spectrum& xyz) {
    const vector3f rgb_vec = matrix3f(
        { 3.240479f, -1.537150f, -0.498535f },
        { -0.969256f, 1.875991f, 0.041556f  },
        { 0.055648f, -0.204043f, 1.057311f  }
        ).t().dot(vector3f{ xyz.x(), xyz.y(), xyz.z() });
    return rgb_spectrum(rgb_vec.x, rgb_vec.y, rgb_vec.z);
  }

  inline xyz_spectrum rgb_to_xyz(const rgb_spectrum& rgb) {
    const vector3f xyz_vec = matrix3f(
        { 0.4124533 , 0.35757984, 0.18042262 },
        { 0.21267127, 0.71515972, 0.07216883 },
        { 0.01933384, 0.11919363, 0.95022693 }
        ).t().dot(vector3f{ rgb.r(), rgb.g(), rgb.b() });
    return xyz_spectrum(xyz_vec.x, xyz_vec.y, xyz_vec.z);
  }

  class sampled_spectrum : public nspectrum {
    public:
      static const int LAMBDA_START = 400;
      static const int LAMBDA_END = 700;
      static const int N_SPECTRAL_SAMPLES = 60;

      sampled_spectrum(Float v = 0);
      sampled_spectrum(std::vector<spectral_sample> samples);
      sampled_spectrum(const nspectrum& nsp) : nspectrum(nsp) {}
      explicit sampled_spectrum(const rgb_spectrum& rgb, bool illuminant = false);

      xyz_spectrum xyz() const;
      inline rgb_spectrum rgb() const { return xyz_to_rgb(xyz()); }

      static Float average_spectral_samples(
          const std::vector<spectral_sample>& samples,
          const Float lambda0,
          const Float lambda1
          );
      static std::vector<spectral_sample> create_spectral_samples(
          const std::vector<Float>& lambdas,
          const std::vector<Float>& values
          );

      static const spectrum_type type = SAMPLED;
  };

  /*
   * Bilinear interpolation -- sp_nb mappings are as follows:
   *    [0] -> 00
   *    [1] -> 01
   *    [2] -> 10
   *    [3] -> 11
   */
  template <class spectrum>
    inline spectrum bilerp(
        const point2f& p,
        const point2f& p_min,
        const point2f& p_max,
        const spectrum sp_nb[4])
    {
      const point2f size(p_max - p_min);
      const Float s = clamp((p.x - p_min.x) / size.x, Float(0), Float(1));
      const Float t = clamp((p.y - p_min.y) / size.y, Float(0), Float(1));
      return (1 - s) * (1 - t) * sp_nb[0] + (1 - s) * t * sp_nb[1]
        + s * (1 - t) * sp_nb[2] + s * t * sp_nb[3];
    }

  extern const std::vector<Float> CIE_LAMBDAS;
  extern const std::vector<spectral_sample> CIE_X;
  extern const std::vector<spectral_sample> CIE_Y;
  extern const std::vector<spectral_sample> CIE_Z;

  extern const sampled_spectrum SMC_X;
  extern const sampled_spectrum SMC_Y;
  extern const sampled_spectrum SMC_Z;

  extern const std::vector<Float> RGB_ADAPTER_LAMBDAS;

  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_WHITE;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_CYAN;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_MAGENTA;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_YELLOW;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_RED;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_GREEN;
  extern const std::vector<spectral_sample> RGB_ADAPTER_SAMPLES_BLUE;

  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_WHITE;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_CYAN;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_MAGENTA;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_YELLOW;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_RED;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_GREEN;
  extern const std::vector<spectral_sample> RGB_ADAPTER_ILLUM_SAMPLES_BLUE;

  extern const sampled_spectrum RGB_ADAPTER_WHITE;
  extern const sampled_spectrum RGB_ADAPTER_CYAN;
  extern const sampled_spectrum RGB_ADAPTER_MAGENTA;
  extern const sampled_spectrum RGB_ADAPTER_YELLOW;
  extern const sampled_spectrum RGB_ADAPTER_RED;
  extern const sampled_spectrum RGB_ADAPTER_GREEN;
  extern const sampled_spectrum RGB_ADAPTER_BLUE;

  extern const sampled_spectrum RGB_ADAPTER_ILLUM_WHITE;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_CYAN;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_MAGENTA;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_YELLOW;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_RED;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_GREEN;
  extern const sampled_spectrum RGB_ADAPTER_ILLUM_BLUE;
} /* namespace tracer */

#endif /* TRACER_SPECTRUM_HPP */
