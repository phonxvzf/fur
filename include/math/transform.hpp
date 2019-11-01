#ifndef MATH_TRANSFORM_HPP
#define MATH_TRANSFORM_HPP

#include "math/quaternion.hpp"
#include "tracer/ray.hpp"
#include "tracer/bounds.hpp"

namespace math {
  namespace tf {

    using namespace tracer;

    class transform {
      public:
        const matrix4f mat;
        const matrix4f mat_inv;

        transform();
        transform(const matrix4f& mat);
        transform(const matrix4f& mat, const matrix4f& mat_inv);

        bool hand_swapped() const;

        transform inverse() const;

        transform operator*(const transform& t) const;

        vector3f  operator()(const vector3f& v) const;
        point3f   operator()(const point3f& p) const;
        normal3f  operator()(const normal3f& n) const;
        ray       operator()(const ray& r) const;
        bounds3f  operator()(const bounds3f& b) const;
    };

    vector3f apply(const matrix4f& tf_mat, const vector3f& pt);
    point3f apply(const matrix4f& tf_mat, const point3f& pt);
    vector3f apply_normal(const matrix4f& tf_mat, const vector3f& normal);
    ray apply(const matrix4f& tf_mat, const ray& r);

    extern transform identity;

    inline transform translate(const vector3f& ds) {
      return matrix4f(
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          vector4f(ds, 1)
          );
    }

    inline transform scale(const vector3f& ss) {
      return matrix4f(
          { ss.x, 0, 0, 0 },
          { 0, ss.y, 0, 0 },
          { 0, 0, ss.z, 0 },
          { 0, 0, 0, 1 }
          );
    }

    transform rotate(const vector3f& axis_n, Float rad);
    transform ndc_to_raster(const vector2i& img_res, const vector2f& ndc_res);
    transform look_at(const vector3f& at, const vector3f& cam_pos, const vector3f& world_up);
    transform ortho(Float z_near, Float z_far);
    transform persp(Float z_near, Float z_far, Float fovy, Float aspect_ratio);
  }
}

#endif /* MATH_TRANSFORM_HPP */
