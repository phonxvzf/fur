#include "tracer/texture.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

namespace tracer {
  texture::texture(const std::string& fpath) {
    stbi_set_flip_vertically_on_load(true);
    int n_channels;
    float* pixels = stbi_loadf(fpath.c_str(), &width, &height, &n_channels, 0);
    int n_pixels = width * height;
    if (pixels == nullptr) {
      std::cerr << "error: could not load " + fpath << ": "
        << stbi_failure_reason() << std::endl;
      std::exit(1);
    }
    spectrums = new rgb_spectrum[n_pixels];
    for (int i = 0; i < 3 * n_pixels; i += 3) {
      spectrums[i / 3] = rgb_spectrum(
          pixels[i],
          pixels[i+1],
          pixels[i+2]
          ).clamp(-50000, 50000);
    }
    stbi_image_free(pixels);
  }

  texture::~texture() {
    if (spectrums != nullptr) delete[] spectrums;
  }

  rgb_spectrum texture::sample(const point2f& st) const {
    // TODO: interpolation
    const point2i p(st.x * width, st.y * height);
    return spectrums[width * p.y + p.x];
  }
}
