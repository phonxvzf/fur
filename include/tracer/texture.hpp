#ifndef TRACER_TEXTURE_HPP
#define TRACER_TEXTURE_HPP

#include <string>
#include <memory>

#include "spectrum.hpp"

namespace tracer {
  class texture {
    private:
      rgb_spectrum* spectrums = nullptr;
      int width, height;

    public:
      texture(const std::string& fpath);
      ~texture();

      rgb_spectrum sample(const point2f& st) const;
      rgb_spectrum sample(const point3f& sph_coords) const;
  };
}

#endif /* TRACER_TEXTURE_HPP */
