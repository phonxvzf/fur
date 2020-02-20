#include "tracer/shapes/cubic_bezier.hpp"
#include "math/sampler.hpp"

namespace tracer {
  namespace shapes {
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

    cubic_bezier::cubic_bezier(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f cps[4],
        Float thickness0,
        Float thickness1
        ) : shape(shape_to_world, surface), thickness0(2 * thickness0), thickness1(2 * thickness1)
    {
      control_points[0] = cps[0];
      control_points[1] = cps[1];
      control_points[2] = cps[2];
      control_points[3] = cps[3];
    }

    cubic_bezier::cubic_bezier(
        const tf::transform& shape_to_world,
        const std::shared_ptr<material>& surface,
        const point3f catmullrom_cps[4],
        Float thickness0,
        Float thickness1,
        cps_position position
        ) : shape(shape_to_world, surface), thickness0(2 * thickness0), thickness1(2 * thickness1)
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
        .expand(0.5 * std::max(thickness0, thickness1));
    }

    bool cubic_bezier::intersect_shape(
        const ray& r,
        const intersect_opts& options,
        intersect_result* result) const
    {
      vector3f basis0, basis1;
      random::rng rng;
      sampler::sample_orthogonals(r.dir, &basis0, &basis1, rng);
      const tf::transform proj(tf::look_at(r.origin + r.dir, r.origin, basis0).inverse());
      const point3f cps[4] = {
        proj(control_points[0]),
        proj(control_points[1]),
        proj(control_points[2]),
        proj(control_points[3])
      };

      result->t_hit = std::numeric_limits<Float>::max();
      const bool hit = intersect_recursive(r, result, cps, 0, 1, wang_depth(cps));
      if (hit) {
        const tf::transform proj_inv = proj.inverse();
        result->hit_point = tf_shape_to_world(result->hit_point);
        result->normal = tf_shape_to_world(proj_inv(result->normal)).normalized();
      }
      return hit;
    }

    bool cubic_bezier::intersect_recursive(
        const ray& r,
        intersect_result* result,
        const point3f cps[4],
        Float u_min,
        Float u_max,
        int depth
        ) const
    {
      const Float max_thickness = std::max(
          math::lerp(u_min, thickness0, thickness1),
          math::lerp(u_max, thickness0, thickness1));
      const Float half_max_thickness = 0.5 * max_thickness;
      const bounds3f curve_bounds = bounds3f(cps[0])
        .merge(cps[1])
        .merge(cps[2])
        .merge(cps[3])
        .expand(half_max_thickness);
      if (!curve_bounds.intersect(ray({ 0, 0, 0 }, { 0, 0, 1 }, r.t_max)))
        return false;

      if (depth == 0) {
        const vector3f dir = cps[3] - cps[0];
        const vector3f dp0 = cps[1] - cps[0];
        const vector3f dp3 = cps[3] - cps[2];
        if (dotproj(dp0, -cps[0]) < 0) return false;
        if (dotproj(dp3, cps[3]) < 0) return false;

        Float w = pow2(dir.x) + pow2(dir.y);
        if (COMPARE_EQ(w, 0)) return false;
        w = math::clamp(-dotproj(cps[0], dir) / w, Float(0), Float(1));

        const point3f p = evaluate(w, cps);
        const Float u = math::lerp(w, u_min, u_max);
        const Float half_thickness = 0.5 * math::lerp(u, thickness0, thickness1);
        const Float dist2 = pow2(p.x) + pow2(p.y);
        if (dist2 > pow2(half_thickness) * 0.25)
          return false;

        // calculate t and normal
        const vector2f tangent(evaluate_differential(w, cps));
        const Float dist = std::sqrt(dist2);
        const Float inv_half_thickness = 1 / half_thickness;
        const vector3f down(-tangent.y, tangent.x, 0);
        Float v = tangent.x * -p.y + tangent.y * p.x > 0 ?
          0.5f + dist * inv_half_thickness     // upper
          : 0.5f - dist * inv_half_thickness;  // lower
        v = math::clamp(v, Float(0), Float(1));

        const Float offset = half_thickness * std::sin(math::lerp(v, 0, PI));
        const Float t = r.medium == INSIDE ? p.z + offset : p.z - offset;
        if (t < 0) return false;

        const normal3f normal(tf::rotate(tangent, radians(math::lerp(v, 0, 180)))(down));

        if (t < result->t_hit) {
          result->t_hit = t;
          result->object = this;
          result->hit_point = r(t);
          result->normal = r.medium == INSIDE ? normal : -normal;
          return true;
        }

        return false;
      }

      // recursively split the curve
      const Float u_mid = (u_min + u_max) * 0.5;
      const point3f cp_split[7] = {
        blossom({ 0, 0, 0 }, cps),
        blossom({ 0, 0, 0.5 }, cps),
        blossom({ 0, 0.5, 0.5 }, cps),
        blossom({ 0.5, 0.5, 0.5 }, cps), // mutual control point
        blossom({ 0.5, 0.5, 1 }, cps),
        blossom({ 0.5, 1, 1 }, cps),
        blossom({ 1, 1, 1 }, cps)
      };
      return intersect_recursive(r, result, &cp_split[0], u_min, u_mid, depth - 1)
        || intersect_recursive(r, result, &cp_split[3], u_mid, u_max, depth - 1);
    } /* intersect_recursive() */

  } /* namespace shapes */
} /* namespace tracer */
