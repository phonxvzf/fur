#include "tracer/hair.hpp"

namespace tracer {
  hair::hair(const std::string& fpath) {
    int result = cyhair.LoadFromFile(fpath.c_str());
    switch( result ) {
      case CY_HAIR_FILE_ERROR_CANT_OPEN_FILE:
        throw std::runtime_error("cannot open hair file");
      case CY_HAIR_FILE_ERROR_CANT_READ_HEADER:
        throw std::runtime_error("cannot read header");
      case CY_HAIR_FILE_ERROR_WRONG_SIGNATURE:
        throw std::runtime_error("wrong signature");
      case CY_HAIR_FILE_ERROR_READING_SEGMENTS:
        throw std::runtime_error("cannot read hair segments");
      case CY_HAIR_FILE_ERROR_READING_POINTS:
        throw std::runtime_error("cannot read hair points");
      case CY_HAIR_FILE_ERROR_READING_COLORS:
        throw std::runtime_error("cannot read hair colors");
      case CY_HAIR_FILE_ERROR_READING_THICKNESS:
        throw std::runtime_error("cannot read hair thickness");
      case CY_HAIR_FILE_ERROR_READING_TRANSPARENCY:
        throw std::runtime_error("cannot read hair transparency");
      default:
        break;
    }

    cyhair_header = cyhair.GetHeader();

    points          = cyhair.GetPointsArray();
    segments_count  = cyhair.GetSegmentsArray();
    colors          = cyhair.GetColorsArray();
    thickness       = cyhair.GetThicknessArray();

    segments_offset = new size_t[cyhair_header.point_count];
    segments_offset[0] = 0;
    for (size_t i = 1; i < cyhair_header.hair_count; ++i) {
      const uint16_t n_segments = segments_count ? segments_count[i-1] : cyhair_header.d_segments;
      segments_offset[i] = segments_offset[i-1] + n_segments + 1; // we skip trailing point
    }
  }

  hair::~hair() {
    delete[] segments_offset;
  }

  void hair::to_beziers(
      std::vector<std::shared_ptr<shape>>& curves,
      const tf::transform& shape_to_world,
      const std::shared_ptr<material>& surface,
      size_t n_strands,
      Float thickness_scale
      ) const
  {
    if (n_strands == 0) n_strands = cyhair_header.hair_count;
    else n_strands = math::clamp(n_strands, 0UL, (size_t) cyhair_header.hair_count);

    for (size_t i = 0; i < n_strands; ++i) {
      const uint16_t n_segments = segments_count ? segments_count[i] : cyhair_header.d_segments;
      for (uint16_t local_segment_id = 0; local_segment_id < n_segments; ++local_segment_id) {
        const size_t offset = segments_offset[i] + local_segment_id;
        point3f catmullrom_cps[4];

        shapes::cubic_bezier::cps_position mode = shapes::cubic_bezier::BODY;
        int head_id, tail_id;
        if (cyhair_header.point_count == 2) {
          // straight line
          catmullrom_cps[0] = point_at(offset);
          catmullrom_cps[3] = point_at(offset + 1);
          catmullrom_cps[1] = lerp(0.25f, catmullrom_cps[0], catmullrom_cps[3]);
          catmullrom_cps[2] = lerp(0.75f, catmullrom_cps[0], catmullrom_cps[3]);
          head_id = offset;
          tail_id = offset + 1;
        } else if (local_segment_id == 0) {
          // head
          mode = shapes::cubic_bezier::HEAD;
          catmullrom_cps[0] = { 0, 0, 0 };
          catmullrom_cps[1] = point_at(offset);
          catmullrom_cps[2] = point_at(offset + 1);
          catmullrom_cps[3] = point_at(offset + 2);
          head_id = offset;
          tail_id = offset + 2;
        } else if (local_segment_id == n_segments - 1) {
          // tail
          mode = shapes::cubic_bezier::TAIL;
          catmullrom_cps[0] = point_at(offset - 1);
          catmullrom_cps[1] = point_at(offset);
          catmullrom_cps[2] = point_at(offset + 1);
          catmullrom_cps[3] = { 0, 0, 0 };
          head_id = offset - 1;
          tail_id = offset + 1;
        } else {
          // body
          catmullrom_cps[0] = point_at(offset - 1);
          catmullrom_cps[1] = point_at(offset);
          catmullrom_cps[2] = point_at(offset + 1);
          catmullrom_cps[3] = point_at(offset + 2);
          head_id = offset - 1;
          tail_id = offset + 2;
        }

        const Float thickness0 = thickness ? thickness[head_id] : cyhair_header.d_thickness;
        const Float thickness1 = thickness ? thickness[tail_id] : cyhair_header.d_thickness;
        curves.emplace_back(new shapes::cubic_bezier(
              shape_to_world,
              surface,
              catmullrom_cps,
              thickness_scale * thickness0,
              thickness_scale * thickness1,
              mode)
            );
      }
    }
  } /* to_beziers() */
} /* namespace tracer */
