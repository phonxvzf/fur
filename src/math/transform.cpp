#include "math/transform.hpp"
#include <iostream>

namespace math {
  namespace tf {

    vector3f apply(const matrix4f& tf_mat, const vector3f& pt) {
      const vector4f result = tf_mat.dot(vector4f(pt, 1.0));
      return vector3f(result) / result.w;
    }

    tracer::ray apply(const matrix4f& tf_mat, const tracer::ray& r) {
      return tracer::ray(tf::apply(tf_mat, r.origin), tf::apply(tf_mat, r.dir));
    }

    matrix4f rotate(const vector3f& axis_n, Float rad) {
      return quat(std::cos(rad / 2), std::sin(rad / 2) * axis_n).to_matrix();
    }

    matrix4f ndc_to_raster(const vector2i& img_res) {
      return scale(vector3f(0.5 * img_res.x, 0.5 * img_res.y, 1)) * translate({ 1, 1, 0 });
    }

    matrix4f look_at(const vector3f& at, const vector3f& cam_pos, const vector3f& world_up) {
      const vector3f dir = (at - cam_pos).normalized();
      const vector3f right = world_up.normalized().cross(dir);
      const vector3f up = dir.cross(right);
      return matrix4f(right, up, dir, vector4f(cam_pos, 1.0));
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
