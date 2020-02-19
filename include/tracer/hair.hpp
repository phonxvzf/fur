#ifndef TRACER_HAIR_HPP
#define TRACER_HAIR_HPP

#include "shapes/cubic_bezier.hpp"
#include "cyHairFile.h"

namespace tracer {
  class hair {
    private:
      cyHairFile cyhair;
      cyHairFile::Header cyhair_header;
      unsigned short* segments_count;
      size_t* segments_offset;
      float* colors;
      float* points;
      float* thickness;

      inline point3f point_at(size_t id) const {
        id = 3 * id;
        return right_to_left({ points[id], points[id+1], points[id+2] });
      }

    public:
      hair(const std::string& fpath);
      ~hair();

      void to_beziers(
          std::vector<std::shared_ptr<shape>>& curves,
          const tf::transform& shape_to_world,
          const std::shared_ptr<material>& surface,
          size_t n_strands = 0,
          Float thickness_scale = 1
          ) const;
  };
}

#endif /* TRACER_HAIR_HPP */
