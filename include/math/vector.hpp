#ifndef MATH_VECTOR_HPP
#define MATH_VECTOR_HPP

namespace math {
  template <typename T, int N>
    class vector_tmpl {
      public:
        T data[N];
    };

  template <typename T>
    class vector3 {
    };
}

#endif /* MATH_VECTOR_HPP */
