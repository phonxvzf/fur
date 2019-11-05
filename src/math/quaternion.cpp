#include "math/quaternion.hpp"

namespace math {
  quat::quat(Float x, Float y, Float z, Float w) : a(x), v({ y, z, w }) {}
  quat::quat(Float a, const vector3f& v) : a(a), v(v) {}
  quat::quat(const quat& q) : a(q.a), v(q.v) {}

  quat& quat::operator=(const quat& q) {
    a = q.a;
    v = q.v;
    return *this;
  }

  quat quat::operator+(const quat& q) const {
    return quat(a + q.a, v + q.v);
  }

  quat quat::operator-(const quat& q) const {
    return quat(a - q.a, v - q.v);
  }

  quat quat::operator*(const quat& q) const {
    return quat(a * q.a - v.dot(q.v), a * q.v + q.a * v + v.cross(q.v));
  }

  quat quat::operator/(Float s) const {
    ASSERT(!COMPARE_EQ(s, 0));
    s = 1 / s;
    return quat(a * s, v * s);
  }

  Float quat::size() const {
    return std::sqrt(a * a + v.size_sq());
  }

  Float quat::size_sq() const {
    return a * a + v.size_sq();
  }

  quat quat::normalized() const {
    Float sz = size();
    ASSERT(!COMPARE_EQ(sz, 0));
    return *this / sz;
  }

  quat quat::conjugate() const {
    return quat(a, -v);
  }

  quat quat::t() const {
    return conjugate();
  }

  quat quat::inverse() const {
    Float sz = size_sq();
    ASSERT(!COMPARE_EQ(sz, 0));
    return conjugate() / sz;
  }

  matrix4f quat::to_matrix() const {
    const quat uq(normalized());
    const Float r = uq.a;
    const Float x = uq.v.x, y = uq.v.y, z = uq.v.z;
    return matrix4f(
        { 1 - 2 * (y * y + z * z), 2 * (x * y + z * r), 2 * (x * z - y * r), 0 },
        { 2 * (x * y - z * r), 1 - 2 * (x * x + z * z), 2 * (y * z + x * r), 0 },
        { 2 * (x * z + y * r), 2 * (y * z + x * r), 1 - 2 * (x * x + y * y), 0 },
        { 0, 0, 0, 1 }
        );
  }

  quat operator*(Float s, const quat& q) {
    return quat(q.a * s, q.v * s);
  }

  quat operator*(const quat& q, Float s) {
    return quat(q.a * s, q.v * s);
  }
} /* namespace math */
