#ifndef TRACER_CAMERA_HPP
#define TRACER_CAMERA_HPP

#include "math/float.hpp"
#include "math/matrix.hpp"

namespace tracer {
  namespace camera {

    using namespace math;

    class projective {
      protected:
        matrix4f transform_mat;

      public:
        vector3f position;

        projective(const matrix4f& transform_mat = matrix4f(0.0f))
          : transform_mat(transform_mat) {}

        virtual vector2f project(const vector3f& point) const {
          vector4f pt = transform_mat * vector4f(point, 1.0);
          return vector2f(pt) / pt.w;
        }

        matrix4f projection_mat() const {
          return transform_mat;
        }
    };

    class ortho : public projective {
      public:
        const Float near;
        const Float far;
        const vector2f pmin;
        const vector2f pmax;

        ortho(Float near, Float far, const vector2f& pmin, const vector2f& pmax)
          : near(near), far(far), pmin(pmin), pmax(pmax)
        {
          // u = (x - pmin.x) / (pmax.x - pmin.x)
          // v = (y - pmin.y) / (pmax.y - pmin.y)
          // TODO
          const vector2f sz = pmax - pmin;
          // x -> u
          transform_mat[0][0] = 1 / sz.x;
          transform_mat[0][3] = -pmin.x / sz.x;
          // y -> v
          transform_mat[1][1] = 1 / sz.y;
          transform_mat[1][3] = -pmin.y / sz.y;
        }
    };

    class persp : public projective {
      public:
        const Float near;
        const Float far;
        const Float fovy;
        const Float aspect_ratio;

        persp(Float near, Float far, Float fovy, Float aspect_ratio)
          : near(near), far(far), fovy(fovy), aspect_ratio(aspect_ratio)
        {
          const Float tanhalf = std::tan(fovy / 2);
          // TODO
        }
    };
  }
}

#endif /* TRACER_CAMERA_HPP */
