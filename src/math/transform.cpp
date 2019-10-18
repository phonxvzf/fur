#include "math/transform.hpp"
#include <iostream>

namespace math {
  namespace tf {

    transform::transform() : mat(matrix4f(1.0)), mat_inv(matrix4f(1.0)) {}
    transform::transform(const matrix4f& mat) : mat(mat), mat_inv(mat.inverse()) {}
    transform::transform(const matrix4f& mat, const matrix4f& mat_inv)
      : mat(mat), mat_inv(mat_inv) {}

    transform transform::inverse() const {
      return transform(mat_inv, mat);
    }

    transform transform::operator*(const transform& t) const {
      return transform(mat * t.mat, t.mat_inv * mat_inv);
    }
        
    point3f transform::operator()(const point3f& p) const {
      return apply(mat, p);
    }

    normal3f transform::operator()(const normal3f& n) const {
      return apply_normal(mat, n);
    }

    tracer::ray transform::operator()(const tracer::ray& r) const {
      return apply(mat, r);
    }

    vector3f apply(const matrix4f& tf_mat, const vector3f& pt) {
      const vector4f result = tf_mat.dot(vector4f(pt, 1.0));
      return vector3f(result) / result.w;
    }
    
    vector3f apply_normal(const matrix4f& tf_mat, const vector3f& normal) {
      const vector4f result = tf_mat.inverse().t().dot(vector4f(normal, 1.0));
      return vector3f(result) / result.w;
    }

    tracer::ray apply(const matrix4f& tf_mat, const tracer::ray& r) {
      return tracer::ray(
          tf::apply(tf_mat, r.origin),
          tf::apply(matrix4f(
              tf_mat.col(0),
              tf_mat.col(1),
              tf_mat.col(2),
              { 0, 0, 0, 1 }
              ),
            r.dir
            )
          );
    }

    transform rotate(const vector3f& axis_n, Float rad) {
      return quat(std::cos(rad / 2), std::sin(rad / 2) * axis_n).to_matrix();
    }

    transform ndc_to_raster(const vector2i& img_res, const vector2f& ndc_res) {
      return translate(vector3f(0, img_res.y, 0))
        * scale({ img_res.x / ndc_res.x, -img_res.y / ndc_res.y, 1 })
        * translate(vector3f(0.5f * ndc_res, 0));
    }

    transform look_at(const vector3f& at, const vector3f& cam_pos, const vector3f& world_up) {
      const vector3f dir = (at - cam_pos).normalized();
      const vector3f right = world_up.normalized().cross(dir);
      const vector3f up = dir.cross(right);
      return matrix4f(right, up, dir, vector4f(cam_pos, 1.0));
    }

    transform ortho(Float z_near, Float z_far) {
      return scale({ 1, 1, 1 / (z_far - z_near) }) * translate({ 0.0, 0.0, -z_near });
    }

    transform persp(Float z_near, Float z_far, Float fovy, Float aspect_ratio) {
      assert(z_far > 0 && z_near > 0);
      const Float len = z_far - z_near;
      const Float cot = 1 / std::tan(fovy / 2);
      return matrix4f(
          { cot / aspect_ratio, 0, 0, 0 },
          { 0, cot, 0, 0 },
          { 0, 0, z_far / len, 1 },
          { 0, 0, -z_far * z_near / len, 0 }
          );
    }
  }
}
