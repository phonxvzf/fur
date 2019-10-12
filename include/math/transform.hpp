#ifndef MATH_TRANSFORM_HPP
#define MATH_TRANSFORM_HPP

#include "math/quaternion.hpp"
#include "tracer/ray.hpp"

namespace math {
  namespace tf {

    vector3f apply(const matrix4f& tf_mat, const vector3f& pt) {
      const vector4f result = tf_mat.dot(vector4f(pt, 1.0));
      return vector3f(result) / result.w;
    }

    tracer::ray apply(const matrix4f& tf_mat, const tracer::ray& r) {
      return tracer::ray(tf::apply(tf_mat, r.origin), tf::apply(tf_mat, r.dir));
    }

    matrix4f translate(const vector3f& ds) {
      return matrix4f(
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, 1, 0 },
          vector4f(ds, 1)
          );
    }

    matrix4f scale(const vector3f& ss) {
      return matrix4f(
          { ss.x, 0, 0, 0 },
          { 0, ss.y, 0, 0 },
          { 0, 0, 0, ss.z },
          { 0, 0, 0, 1 }
          );
    }

    matrix4f rotate(const vector3f& axis_n, Float rad) {
      return quat(std::cos(rad / 2), std::sin(rad / 2) * axis_n).to_matrix();
    }

    matrix4f ndc_to_raster(const vector2i& img_res) {
      return translate(vector3f(0, img_res.y, 0))
        * scale(vector3f(0.5 * img_res.x, -0.5 * img_res.y, 1))
        * translate({ 1, 1, 0 });
    }

    matrix4f look_at(const vector3f& at, const vector3f& cam_pos, const vector3f& world_up) {
      const vector3f forward = (cam_pos - at).normalized();
      const vector3f left = forward.cross(world_up.normalized());
      const vector3f up = left.cross(forward);
      const matrix4f mr = matrix4f(left, up, forward, { 0, 0, 0, 1 }).t();
      const matrix4f mt = tf::translate(-cam_pos);
      return mr * mt;
    }

    matrix4f ortho(Float z_near, Float z_far) {
      return scale({ 1.0, 1.0, 1 / (z_far - z_near) }) * translate({ 0.0, 0.0, -z_near });
    }

    matrix4f persp(Float z_near, Float z_far, Float fovy) {
      const Float len = z_far - z_near;
      const matrix4f proj(
          { 1, 0, 0, 0 },
          { 0, 1, 0, 0 },
          { 0, 0, z_far / len, 1 },
          { 0, 0, -z_far * z_near / len, 0 }
          );
      const Float inv_tan = 1 / std::tan(fovy / 2);
      return scale({ inv_tan, inv_tan, 1 }) * proj;
    }
  }
}

#endif /* MATH_TRANSFORM_HPP */
