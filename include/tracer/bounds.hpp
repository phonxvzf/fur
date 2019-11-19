#ifndef TRACER_BOUNDS_HPP
#define TRACER_BOUNDS_HPP

#include "math/vector.hpp"

namespace tracer {
  using namespace math;

  template <typename T> class bounds2 {
    public:
      point2<T> p_min, p_max;

      bounds2(const point2<T>& p_min, const point2<T>& p_max) : p_min(p_min), p_max(p_max) {}
      bounds2(const point2<T>& p) : p_min(p), p_max(p) {}

      bounds2& operator=(const bounds2& cpy) {
        p_min = cpy.p_min;
        p_max = cpy.p_max;
        return *this;
      }

      T width_x() const {
        return p_max.x - p_min.x;
      }

      T width_y() const {
        return p_max.y - p_min.y;
      }

      T width() const {
        return width_x();
      }

      T height() const {
        return width_y();
      }

      T area() const {
        return width() * height();
      }

      bool invalid() const {
        return width() <= 0 || height() <= 0;
      }

      bounds2<T> merge(const bounds2<T>& other) const {
        return bounds2(
            { std::min(p_min.x, other.p_min.x), std::min(p_min.y, other.p_min.y) },
            { std::max(p_max.x, other.p_max.x), std::max(p_max.y, other.p_max.y) }
            );
      }

      bounds2<T> intersect(const bounds2<T>& other) const {
        return bounds2(
            { std::max(p_min.x, other.p_min.x), std::max(p_min.y, other.p_min.y) },
            { std::min(p_max.x, other.p_max.x), std::min(p_max.y, other.p_max.y) }
            );
      }
  };

  template <typename T> class bounds3 {
    public:
      point3<T> p_min, p_max;

      bounds3(const point3<T>& p_min, const point3<T>& p_max) : p_min(p_min), p_max(p_max) {}
      bounds3(const point3<T>& p) : p_min(p), p_max(p) {}
      
      bounds3& operator=(const bounds3& cpy) {
        p_min = cpy.p_min;
        p_max = cpy.p_max;
        return *this;
      }

      T width_x() const {
        return p_max.x - p_min.x;
      }

      T width_y() const {
        return p_max.y - p_min.y;
      }

      T width_z() const {
        return p_max.z - p_min.z;
      }

      T volume() const {
        return width_x() * width_y() * width_z();
      }

      bool invalid() const {
        return width_x() <= 0 || width_y() <= 0 || width_z() <= 0;
      }

      Float diagonal() const {
        return (p_max - p_min).size();
      }

      bounds3<T> merge(const bounds3<T>& other) const {
        return bounds3(
            {
            std::min(p_min.x, other.p_min.x),
            std::min(p_min.y, other.p_min.y),
            std::min(p_min.z, other.p_min.z)
            },
            {
            std::max(p_max.x, other.p_max.x),
            std::max(p_max.y, other.p_max.y),
            std::max(p_max.z, other.p_max.z)
            }
            );
      }

      bounds3<T> intersect(const bounds3<T>& other) const {
        return bounds3(
            {
            std::max(p_min.x, other.p_min.x),
            std::max(p_min.y, other.p_min.y),
            std::max(p_min.z, other.p_min.z)
            },
            {
            std::min(p_max.x, other.p_max.x),
            std::min(p_max.y, other.p_max.y),
            std::min(p_max.z, other.p_max.z)
            }
            );
      }
  };

  typedef bounds2<int> bounds2i;
  typedef bounds3<int> bounds3i;

  typedef bounds2<Float> bounds2f;
  typedef bounds3<Float> bounds3f;
}

#endif /* TRACER_BOUNDS_HPP */
