#include "tracer/shapes/cubic_bezier.hpp"

namespace tracer {
  namespace shapes {
    static const matrix4f CATMULLROM_TO_BEZIER{
      { 0, -1.f / 6, 0, 0 },
      { 1, 1, -1.f / 6, 0 },
      { 0, 1.f / 6, 1, 1  },
      { 0, 0, 1.f / 6, 0  }
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

    cubic_bezier::cubic_bezier(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f catmullrom_cps[4],
        Float thickness
        ) : shape(shape_to_world, surface), thickness(thickness)
    {
      control_points[0] = catmullrom_cps[0];
      control_points[1] = catmullrom_cps[1];
      control_points[2] = catmullrom_cps[2];
      control_points[3] = catmullrom_cps[3];
    }

    cubic_bezier::cubic_bezier(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f catmullrom_cps[4],
        Float thickness,
        cps_position position
        ) : shape(shape_to_world, surface), thickness(thickness)
    {
      switch (position) {
        case HEAD:
          // catmullrom_cps[0] is invalid
          for (int i = 0; i < 3; ++i) {
            const vector4f x = CATMULLROM_TO_BEZIER_HEAD.dot(vector4f(
                  0,
                  catmullrom_cps[1][i],
                  catmullrom_cps[2][i],
                  catmullrom_cps[3][i]
                  ));
            control_points[0][i] = x[0];
            control_points[1][i] = x[1];
            control_points[2][i] = x[2];
            control_points[3][i] = x[3];
          }
          break;
          break;
        case TAIL:
          // catmullrom_cps[3] is invalid
          for (int i = 0; i < 3; ++i) {
            const vector4f x = CATMULLROM_TO_BEZIER_TAIL.dot(vector4f(
                  catmullrom_cps[0][i],
                  catmullrom_cps[1][i],
                  catmullrom_cps[2][i],
                  0
                  ));
            control_points[0][i] = x[0];
            control_points[1][i] = x[1];
            control_points[2][i] = x[2];
            control_points[3][i] = x[3];
          }
          break;
        default:
          for (int i = 0; i < 3; ++i) {
            const vector4f x = CATMULLROM_TO_BEZIER.dot(vector4f(
                  catmullrom_cps[0][i],
                  catmullrom_cps[1][i],
                  catmullrom_cps[2][i],
                  catmullrom_cps[3][i]
                  ));
            control_points[0][i] = x[0];
            control_points[1][i] = x[1];
            control_points[2][i] = x[2];
            control_points[3][i] = x[3];
          }
          break;
      }
    } /* catmullrom2bezier ctor */

    bounds3f cubic_bezier::bounds() const {
      return bounds3f(control_points[0])
        .merge(control_points[1])
        .merge(control_points[2])
        .merge(control_points[3])
        .scale(thickness);
    }

    bool cubic_bezier::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      // TODO

    }
  } /* namespace shapes */
} /* namespace tracer */
