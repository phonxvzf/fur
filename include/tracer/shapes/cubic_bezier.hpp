#ifndef TRACER_SHAPES_CUBIC_BEZIER_HPP
#define TRACER_SHAPES_CUBIC_BEZIER_HPP

#include "tracer/shape.hpp"

namespace tracer {
  namespace shapes {
    class cubic_bezier : public shape {
      private:
        point3f control_points[4];
        const Float thickness0, thickness1;

        bool intersect_recursive(
            const ray& r,
            intersect_result* result,
            const point3f cps[4],
            Float u_min,
            Float u_max,
            int depth
            ) const;

        inline Float wang_term(const Float3& x) const {
          return std::abs(x[0] - 2 * x[1] + x[2]);
        }

        inline int wang_depth(const point3f cps[4]) const {
          Float L0 = 0;
          for (int i = 0; i < 2; ++i) {
            L0 = std::max(L0, max3(
                  wang_term({ cps[i].x, cps[i+1].x, cps[i+2].x }),
                  wang_term({ cps[i].y, cps[i+1].y, cps[i+2].y }),
                  wang_term({ cps[i].z, cps[i+1].z, cps[i+2].z })
                  )
                );
          }
          const Float max_dist_error = std::max(thickness0, thickness1) * 0.05;
          const Float r0 = std::log(SQRT_TWO * 12.f * L0 / (8.f * max_dist_error)) * 0.7213475108f;
          return clamp(static_cast<int>(std::round(r0)), 0, 10);
        }

      public:
        enum cps_position {
          HEAD, BODY, TAIL
        };

        cubic_bezier(
            const tf::transform& shape_to_world,
            const std::shared_ptr<material>& surface,
            const point3f cps[4],
            Float thickness0,
            Float thickness1
            );

        bounds3f bounds() const override;

        bool intersect_shape(
            const ray& r,
            const intersect_opts& options,
            intersect_result* result)
          const override;

        inline static point3f blossom(const Float3& u, const point3f cps[4]) {
          const point3f a0 = lerp(u[0], cps[0], cps[1]);
          const point3f a1 = lerp(u[0], cps[1], cps[2]);
          const point3f a2 = lerp(u[0], cps[2], cps[3]);
          const point3f b0 = lerp(u[1], a0, a1);
          const point3f b1 = lerp(u[1], a1, a2);
          return lerp(u[2], b0, b1);
        }

        inline static point3f evaluate(Float u, const point3f cps[4]) {
          const Float ou = 1 - u;
          return cps[0] * pow3(ou)
            + cps[1] * 3 * u * pow2(ou)
            + cps[2] * 3 * pow2(u) * ou
            + cps[3] * pow3(u);
        }

        // Evaluate dp/du i.e. tangent vector
        inline static vector3f evaluate_differential(Float u, const point3f cps[4]) {
          const Float ou = 1 - u;
          return 3 * pow2(ou) * (cps[1] - cps[0])
            + 6 * u * ou * (cps[2] - cps[1])
            + 3 * pow2(u) * (cps[3] - cps[2]);
        }
    };
  }
}

#endif /* TRACER_SHAPES_CUBIC_BEZIER_HPP */
