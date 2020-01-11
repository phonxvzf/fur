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
      std::cerr << "error: could not load " + fpath << ": "
        << stbi_failure_reason() << std::endl;
      std::exit(1);
    }

    spectrums = new rgb_spectrum[n_pixels];

    #pragma omp parallel for
    for (int i = 0; i < 3 * n_pixels; i += 3) {
      spectrums[i / 3] = rgb_spectrum(
          pixels[i],
          pixels[i+1],
          pixels[i+2]
          ).clamp(-50000, 50000);
    }

    stbi_image_free(pixels);

    std::wcout << L" done" << std::endl;
  }

  texture::~texture() {
    if (spectrums != nullptr) delete[] spectrums;
  }

  rgb_spectrum texture::sample(const point2f& st) const {
    // TODO: interpolation
    const point2i p(max0(st.x * width - 1), max0(st.y * height - 1));
    return spectrums[width * p.y + p.x];
  }

  rgb_spectrum texture::sample(const point3f& sph_coords) const {
    const Float theta = reduce_angle(std::acos(clamp(sph_coords.y, -1, 1)));
    const Float phi = reduce_angle(std::atan2(-sph_coords.z, sph_coords.x));
    return sample(point2f(phi * INV_TWO_PI, theta * INV_PI));
  }
}
