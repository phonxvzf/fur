#include <sstream>
#include "tracer/hair.hpp"

namespace tracer {
  static const matrix4f CATMULLROM_TO_BEZIER{
    { 0, -1.f / 6, 0, 0   },
    { 1, 1, 1.f / 6, 0    },
    { 0, 1.f / 6, 1, 1    },
    { 0, 0, -1.f / 6, 0   }
  };

  static const matrix4f CATMULLROM_TO_BEZIER_HEAD{
    { 0, 0, 0, 0                },
    { 1.f, 0.5f, 1.f / 6, 0     },
    { 0, 2.f / 3, 1, 1          },
    { 0, -1.f / 6, -1.f / 6, 0  }
  };

  static const matrix4f CATMULLROM_TO_BEZIER_TAIL{
    { 0, -1.f / 6, -1.f / 6, 0  },
    { 1, 1, 2.f / 3, 0          },
    { 0, 1.f / 6, 0.5f, 1       },
    { 0, 0, 0, 0                }
  };

  hair::hair(const std::string& fpath) {
    int result = cyhair.LoadFromFile(fpath.c_str());
    switch (result) {
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

    file_path = fpath;

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

    tangents = new float[3 * cyhair_header.point_count];
    if (cyhair.FillDirectionArray(tangents) == 0) {
      throw std::runtime_error("cannot precompute hair tangents");
    }
  }

  hair::~hair() {
    delete[] segments_offset;
    delete[] tangents;
  }

  void hair::catmullrom_to_bezier(
      point3f bezier_cps[4],
      const point3f cps[4],
      cps_position position) const
  {
    switch (position) {
      case HEAD:
        for (int i = 0; i < 3; ++i) {
          const vector4f x = CATMULLROM_TO_BEZIER_HEAD.dot(vector4f(
                0,
                cps[1][i],
                cps[2][i],
                cps[3][i]
                ));
          bezier_cps[0][i] = x[0];
          bezier_cps[1][i] = x[1];
          bezier_cps[2][i] = x[2];
          bezier_cps[3][i] = x[3];
        }
        break;
        break;
      case TAIL:
        for (int i = 0; i < 3; ++i) {
          const vector4f x = CATMULLROM_TO_BEZIER_TAIL.dot(vector4f(
                cps[0][i],
                cps[1][i],
                cps[2][i],
                0
                ));
          bezier_cps[0][i] = x[0];
          bezier_cps[1][i] = x[1];
          bezier_cps[2][i] = x[2];
          bezier_cps[3][i] = x[3];
        }
        break;
      default:
        for (int i = 0; i < 3; ++i) {
          const vector4f x = CATMULLROM_TO_BEZIER.dot(vector4f(
                cps[0][i],
                cps[1][i],
                cps[2][i],
                cps[3][i]
                ));
          bezier_cps[0][i] = x[0];
          bezier_cps[1][i] = x[1];
          bezier_cps[2][i] = x[2];
          bezier_cps[3][i] = x[3];
        }
        break;
    }
  }

  void hair::halve_bezier(point3f left[4], point3f right[4], const point3f cps[4]) const {
    const point3f cpy[4] = { cps[0], cps[1], cps[2], cps[3] };
    left[0] = shapes::cubic_bezier::blossom({ 0, 0, 0 }, cpy);
    left[1] = shapes::cubic_bezier::blossom({ 0, 0, 0.5 }, cpy);
    left[2] = shapes::cubic_bezier::blossom({ 0, 0.5, 0.5 }, cpy);
    left[3] = shapes::cubic_bezier::blossom({ 0.5, 0.5, 0.5 }, cpy);
    right[0] = left[3]; // mutual control point
    right[1] = shapes::cubic_bezier::blossom({ 0.5, 0.5, 1 }, cpy);
    right[2] = shapes::cubic_bezier::blossom({ 0.5, 1, 1 }, cpy);
    right[3] = shapes::cubic_bezier::blossom({ 1, 1, 1 }, cpy);
  }

  bvh_tree* hair::to_beziers(
      std::vector<std::shared_ptr<shape>>& curves,
      const tf::transform& shape_to_world,
      const std::shared_ptr<material>& surface,
      uintptr_t* hair_id,
      size_t n_strands,
      Float thickness_scale,
      bool subbvh,
      bool subdivide
      ) const
  {
    if (n_strands == 0) n_strands = cyhair_header.hair_count;
    else n_strands = math::clamp(n_strands, 0UL, (size_t) cyhair_header.hair_count);

    bvh_tree* strand_bvh = new bvh_tree[n_strands];

    std::wstringstream wss;
    wss << file_path.c_str();
    std::wcout << L"  * Processing hair file " << wss.str() << std::endl;

    size_t n_total_curves = 0;
    for (size_t i = 0; i < n_strands; ++i) {
      const uint16_t n_segments = segments_count ? segments_count[i] : cyhair_header.d_segments;
      size_t n_curves = 0;
      for (uint16_t local_segment_id = 0; local_segment_id < n_segments; ++local_segment_id) {
        const size_t offset = segments_offset[i] + local_segment_id;
        point3f catmullrom_cps[4];

        cps_position mode = BODY;
        int head_id, tail_id;
        if (cyhair_header.point_count == 2) {
          // straight line
          catmullrom_cps[0] = point_at(offset);
          catmullrom_cps[3] = point_at(offset + 1);
          catmullrom_cps[1] = lerp(0.33f, catmullrom_cps[0], catmullrom_cps[3]);
          catmullrom_cps[2] = lerp(0.66f, catmullrom_cps[0], catmullrom_cps[3]);
          head_id = offset;
          tail_id = offset + 1;
        } else if (local_segment_id == 0) {
          // head
          mode = HEAD;
          catmullrom_cps[0] = { 0, 0, 0 };
          catmullrom_cps[1] = point_at(offset);
          catmullrom_cps[2] = point_at(offset + 1);
          catmullrom_cps[3] = point_at(offset + 2);
          head_id = offset;
          tail_id = offset + 2;
        } else if (local_segment_id == n_segments - 1) {
          // tail
          mode = TAIL;
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
        point3f bezier_cps[4];
        catmullrom_to_bezier(bezier_cps, catmullrom_cps, mode);

        point3f sub_curve[8][4]; // subdivide into 8 curves
        halve_bezier(sub_curve[0], sub_curve[4], bezier_cps);
        halve_bezier(sub_curve[0], sub_curve[2], sub_curve[0]);
        halve_bezier(sub_curve[0], sub_curve[1], sub_curve[0]);
        halve_bezier(sub_curve[2], sub_curve[3], sub_curve[2]);
        halve_bezier(sub_curve[4], sub_curve[6], sub_curve[4]);
        halve_bezier(sub_curve[4], sub_curve[5], sub_curve[4]);
        halve_bezier(sub_curve[6], sub_curve[7], sub_curve[6]);

        constexpr Float one_eighth = 0.125f;
        const Float sub_thickness[9] = {
          thickness0,
          math::lerp(one_eighth, thickness0, thickness1),
          math::lerp(2 * one_eighth, thickness0, thickness1),
          math::lerp(3 * one_eighth, thickness0, thickness1),
          math::lerp(4 * one_eighth, thickness0, thickness1),
          math::lerp(5 * one_eighth, thickness0, thickness1),
          math::lerp(6 * one_eighth, thickness0, thickness1),
          math::lerp(7 * one_eighth, thickness0, thickness1),
          local_segment_id + 1 == n_segments ? 0.f : thickness1
        };

        for (size_t j = 0; j < (subdivide ? 8 : 1); ++j) {
          shapes::cubic_bezier* bezier;
          if (subdivide) {
            bezier = new shapes::cubic_bezier(
                shape_to_world,
                surface,
                sub_curve[j],
                thickness_scale * sub_thickness[j],
                thickness_scale * sub_thickness[j+1],
                nullptr
                );
          } else {
            bezier = new shapes::cubic_bezier(
                shape_to_world,
                surface,
                bezier_cps,
                thickness_scale * thickness0,
                thickness_scale * thickness1,
                nullptr
                );
          }
          if (subbvh) {
            bezier->strand_id = i;
            bezier->hair_id = (uintptr_t) this;
          }
          bezier->curve_id = n_curves + n_total_curves;
          curves.push_back(std::shared_ptr<shape>(bezier));
          ++n_curves;
        }
      } /* for local_segment_id */

      if (subbvh) {
        std::wcout << L"\r    * Constructing sub BVH for strand "
          << i+1 << L"/" << n_strands << std::flush;
        strand_bvh[i] = bvh_tree(std::vector<std::shared_ptr<shape>>(
              curves.begin() + n_total_curves,
              curves.begin() + n_total_curves + n_curves
              ));
        n_total_curves += n_curves;
      }
    } /* for i */

    if (subbvh) {
      *hair_id = (uintptr_t) this;
      std::wcout << std::endl;
    }
    return strand_bvh;
  } /* to_beziers() */

} /* namespace tracer */
