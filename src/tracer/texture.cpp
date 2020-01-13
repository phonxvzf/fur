#include <iostream>
#include <sstream>
#include "tracer/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "math/util.hpp"

namespace tracer {
  texture::texture(const std::string& fpath) {
    std::wstringstream wss;
    wss << fpath.c_str();
    std::wcout << L"* Loading environment map " << wss.str() << L"..." << std::flush;

    stbi_set_flip_vertically_on_load(false);

    int n_channels;
    float* pixels = stbi_loadf(fpath.c_str(), &width, &height, &n_channels, 0);
    int n_pixels = width * height;
    if (pixels == nullptr) {
      std::cerr << std::endl << "error: could not load " + fpath << ": "
        << stbi_failure_reason() << std::endl;
      std::exit(1);
    }

    spectrums = new sampled_spectrum[n_pixels];

    for (int i = 0; i < 3 * n_pixels; i += 3) {
      spectrums[i / 3] = sampled_spectrum(
          rgb_spectrum(
            pixels[i],
            pixels[i+1],
            pixels[i+2]
            ).clamp(-50000, 50000),
          true
          );
    }

    stbi_image_free(pixels);

    std::wcout << L" done" << std::endl;
  }

  texture::~texture() {
    if (spectrums != nullptr) delete[] spectrums;
  }

  sampled_spectrum texture::sample(const point2f& st) const {
    const point2i p(max0(st.x * width - 1), max0(st.y * height - 1));
    const point2f pf(p + point2f(0.5f));
    const point2f stc(max0(st.x * width), max0(st.y * height));
    sampled_spectrum sp_nb[4];
    point2f p_min, p_max;
    if (stc.x < pf.x && stc.y < pf.y) {
      p_min = pf - point2f(1, 1);
      p_max = pf;
    } else if (stc.x > pf.x && stc.y < pf.y) {
      p_min = pf - point2f(0, 1);
      p_max = pf + point2f(1, 0);
    } else if (stc.x < pf.x && stc.y > pf.y) {
      p_min = pf - point2f(1, 0);
      p_max = pf + point2f(0, 1);
    } else if (stc.x > pf.x && stc.y > pf.y) {
      p_min = pf;
      p_max = pf + point2f(1, 1);
    } else {
      return spectrum_at(p);
    }

    sp_nb[0] = spectrum_at(point2i(p_min));
    sp_nb[1] = spectrum_at(point2i(p_min) + point2i(0, 1));
    sp_nb[2] = spectrum_at(point2i(p_min) + point2i(1, 0));
    sp_nb[3] = spectrum_at(point2i(p_min) + point2i(1, 1));

    return bilerp(stc, p_min, p_max, sp_nb);
  }

  sampled_spectrum texture::sample(const point3f& sph_coords) const {
    const Float theta = reduce_angle(std::acos(clamp(sph_coords.y, Float(-1), Float(1))));
    const Float phi   = reduce_angle(std::atan2(-sph_coords.z, sph_coords.x));
    return sample(point2f(phi * INV_TWO_PI, theta * INV_PI));
  }
}
