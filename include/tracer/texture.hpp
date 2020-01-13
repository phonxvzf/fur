#ifndef TRACER_TEXTURE_HPP
#define TRACER_TEXTURE_HPP

#include <string>
#include <memory>

#include "spectrum.hpp"

namespace tracer {
  class texture {
    private:
      sampled_spectrum* spectrums = nullptr;
      int width, height;

      inline sampled_spectrum spectrum_at(const point2i& sti) const {
        const point2i stc(clamp(sti.x, 0, width - 1), clamp(sti.y, 0, height - 1));
        return spectrums[width * stc.y + stc.x];
      }

    public:
      texture(const std::string& fpath);
      ~texture();

      sampled_spectrum sample(const point2f& st) const;
      sampled_spectrum sample(const point3f& sph_coords) const;
  };
}

#endif /* TRACER_TEXTURE_HPP */
