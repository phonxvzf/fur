#include "math/transform.hpp"
#include <iostream>

namespace math {
  namespace tf {

    transform::transform() : mat(matrix4f(1.0)), mat_inv(matrix4f(1.0)) {}
    transform::transform(const matrix4f& mat) : mat(mat), mat_inv(mat.inverse()) {}
    transform::transform(const matrix4f& mat, const matrix4f& mat_inv)
      : mat(mat), mat_inv(mat_inv) {}
        
    transform& transform::operator=(const transform& t) {
      mat = t.mat;
      mat_inv = t.mat_inv;
      return *this;
    }

    bool transform::hand_swapped() const {
      const Float det = matrix3f(
          mat.col(0),
          mat.col(1),
          mat.col(2)
          ).det();
      return det < 0;
    }

    transform transform::inverse() const {
      return transform(mat_inv, mat);
    }

    transform transform::operator*(const transform& t) const {
      return transform(mat * t.mat, t.mat_inv * mat_inv);
    }

    vector3f transform::operator()(const vector3f& v) const {
      return apply(mat, v);
    }
        
    point3f transform::operator()(const point3f& p) const {
      return apply(mat, p);
    }

    normal3f transform::operator()(const normal3f& n) const {
      const vector4f result = mat_inv.t().dot(vector4f(n, 0));
      return normal3f(result);
    }

    ray transform::operator()(const ray& r) const {
      return apply(mat, r);
    }
 
    bounds3f transform::operator()(const bounds3f& b) const {
      bounds3f new_bounds((*this)(point3f(b.p_max.x, b.p_max.y, b.p_max.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_min.x, b.p_min.y, b.p_min.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_min.x, b.p_min.y, b.p_max.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_min.x, b.p_max.y, b.p_min.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_min.x, b.p_max.y, b.p_max.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_max.x, b.p_min.y, b.p_min.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_max.x, b.p_min.y, b.p_max.z)));
      new_bounds = new_bounds.merge((*this)(point3f(b.p_max.x, b.p_max.y, b.p_min.z)));
      return new_bounds;
    }

    vector3f apply(const matrix4f& tf_mat, const vector3f& vec) {
      const vector4f result = tf_mat.dot(vector4f(vec, 0));
      return vector3f(result);
    }

    point3f apply(const matrix4f& tf_mat, const point3f& pt) {
      const vector4f result = tf_mat.dot(vector4f(pt, 1));
      return vector3f(result) / result.w;
    }
    
    normal3f apply_normal(const matrix4f& tf_mat, const vector3f& normal) {
      const vector4f result = tf_mat.inverse().t().dot(vector4f(normal, 0));
      return normal3f(result);
    }

    ray apply(const matrix4f& tf_mat, const ray& r) {
      const Float t_max_scale = vector3f(tf_mat[0][0], tf_mat[1][1], tf_mat[2][2]).size();
      return ray(
          tf::apply(tf_mat, r.origin),
          tf::apply(tf_mat, r.dir),
          r.t_max * t_max_scale,
          r.medium
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
      const vector3f right = world_up.cross(dir).normalized();
      const vector3f up = dir.cross(right).normalized();
      return matrix4f(right, up, dir, vector4f(cam_pos, 1.0));
    }

    transform ortho(Float z_near, Float z_far) {
      return scale({ 1, 1, 1 / (z_far - z_near) }) * translate({ 0.0, 0.0, -z_near });
    }

    transform persp(Float z_near, Float z_far, Float fovy, Float aspect_ratio) {
      ASSERT(z_far > 0 && z_near > 0);
      const Float len = z_far - z_near;
      const Float cot = 1 / std::tan(fovy / 2);
      return matrix4f(
          { cot / aspect_ratio, 0, 0, 0 },
          { 0, cot, 0, 0 },
          { 0, 0, z_far / len, 1 },
          { 0, 0, -z_far * z_near / len, 0 }
          );
    }

    transform identity(matrix4f(1.0f));

  } /* namespace tf */
} /* namespace math */
