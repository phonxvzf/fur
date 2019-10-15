#ifndef MATH_TRANSFORM_HPP
#define MATH_TRANSFORM_HPP

#include "math/quaternion.hpp"
#include "tracer/ray.hpp"

namespace math {
  namespace tf {

    vector3f apply(const matrix4f& tf_mat, const vector3f& pt);
    tracer::ray apply(const matrix4f& tf_mat, const tracer::ray& r);

    inline matrix4f translate(const vector3f& ds) {
      return matrix4f(
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          vector4f(ds, 1)
          );
    }

    inline matrix4f scale(const vector3f& ss) {
      return matrix4f(
          { ss.x, 0, 0, 0 },
          { 0, ss.y, 0, 0 },
          { 0, 0, ss.z, 0 },
          { 0, 0, 0, 1 }
          );
    }

    matrix4f rotate(const vector3f& axis_n, Float rad);
    matrix4f ndc_to_raster(const vector2i& img_res);
    matrix4f look_at(const vector3f& at, const vector3f& cam_pos, const vector3f& world_up);
    matrix4f ortho(Float z_near, Float z_far);
    matrix4f persp(Float z_near, Float z_far, Float fovy);
  }
}

#endif /* MATH_TRANSFORM_HPP */
