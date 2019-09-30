#include <iostream>
#include <cassert>

#include "math/vector.hpp"

using namespace math;

void assert_vec(const vector2i& v, int x) {
  assert((v.x == x) && (v.y == x));
}

void assert_vec(const vector3i& v, int x) {
  assert((v.x == x) && (v.y == x) && (v.z == x));
}

void assert_vec(const vector4i& v, int x) {
  assert((v.x == x) && (v.y == x) && (v.z == x) && (v.w == x));
}

void assert_vec(const vector2i& v) {
  assert((v.x == 1) && (v.y == 2));
}

void assert_vec(const vector3i& v) {
  assert((v.x == 1) && (v.y == 2) && (v.z == 3));
}

void assert_vec(const vector4i& v) {
  assert((v.x == 1) && (v.y == 2) && (v.z == 3) && (v.w == 4));
}

void test_constructors() {
  // test default constructors
  assert_vec(vector2i(), 0);
  assert_vec(vector3i(), 0);
  assert_vec(vector4i(), 0);

  // test element constructors
  assert_vec(vector2i(1, 2));
  assert_vec(vector3i(1, 2, 3));
  assert_vec(vector4i(1, 2, 3, 4));

  // test copy constructors
  assert_vec(vector2i(vector2i(1, 2)));
  assert_vec(vector2i(vector3i(1, 2, 3)));
  assert_vec(vector2i(vector4i(1, 2, 3, 4)));

  assert_vec(vector3i(vector2i(0, 0)), 0);
  assert_vec(vector3i(vector3i(1, 2, 3)));
  assert_vec(vector3i(vector4i(1, 2, 3, 4)));

  assert_vec(vector4i(vector2i(0, 0)), 0);
  assert_vec(vector4i(vector3i(0, 0, 0)), 0);
  assert_vec(vector4i(vector4i(1, 2, 3, 4)));

  // test assignment operators
  vector2i vec2 = vector2i(1, 2);
  assert_vec(vec2);
  vector3i vec3 = vector3i(1, 2, 3);
  assert_vec(vec3);
  vector4i vec4 = vector4i(1, 2, 3, 4);
  assert_vec(vec4);
}

void test_add() {
  assert_vec(vector2i(1, 2) + vector2i(-1, -2), 0);
  assert_vec(vector3i(1, 2, 3) + vector3i(-1, -2, -3), 0);
  assert_vec(vector4i(1, 2, 3, 4) + vector4i(-1, -2, -3, -4), 0);

  vector2i vec2;
  vec2 += vector2i(1, 2);
  assert_vec(vec2);
  vector3i vec3;
  vec3 += vector3i(1, 2, 3);
  assert_vec(vec3);
  vector4i vec4;
  vec4 += vector4i(1, 2, 3, 4);
  assert_vec(vec4);
}

void test_minus() {
  vector2i vec2 = -vector2i(1);
  assert_vec(vec2, -1);
  vector3i vec3 = -vector3i(1);
  assert_vec(vec3, -1);
  vector4i vec4 = -vector4i(1);
  assert_vec(vec4, -1);

  assert_vec(vector2i(1, 2) - vector2i(1, 2), 0);
  assert_vec(vector3i(1, 2, 3) - vector3i(1, 2, 3), 0);
  assert_vec(vector4i(1, 2, 3, 4) - vector4i(1, 2, 3, 4), 0);

  vec2 -= vector2i(-1);
  vec3 -= vector3i(-1);
  vec4 -= vector4i(-1);
  assert_vec(vec2, 0);
  assert_vec(vec3, 0);
  assert_vec(vec4, 0);
}

void test_elm_mult() {
  assert_vec(vector2i(3, 4) * vector2i(4, 3), 12);
  assert_vec(vector3i(3, 4, 5) * vector3i(20, 15, 12), 60);
  assert_vec(vector4i(3, 4, 5, 6) * vector4i(20, 15, 12, 10), 60);
}

void test_elm_div() {
  assert_vec(vector2i(3, 4) / vector2i(3, 4), 1);
  assert_vec(vector3i(3, 4, 5) / vector3i(3, 4, 5), 1);
  assert_vec(vector4i(3, 4, 5, 6) / vector4i(3, 4, 5, 6), 1);
}

void test_scalar_mult() {
  assert_vec(vector2i(1) * 5, 5);
  assert_vec(5 * vector2i(1), 5);
  assert_vec(vector3i(1) * 5, 5);
  assert_vec(5 * vector3i(1), 5);
  assert_vec(vector4i(1) * 5, 5);
  assert_vec(5 * vector4i(1), 5);

  vector2i vec2(1);
  vec2 *= 5;
  assert_vec(vec2, 5);
  vector3i vec3(1);
  vec3 *= 5;
  assert_vec(vec3, 5);
  vector4i vec4(1);
  vec4 *= 5;
  assert_vec(vec4, 5);
}

void test_scalar_div() {
  assert_vec(vector2i(10) / 5, 2);
  assert_vec(vector3i(10) / 5, 2);
  assert_vec(vector4i(10) / 5, 2);

  vector2i vec2(5);
  vec2 /= 5;
  assert_vec(vec2, 1);
  vector3i vec3(5);
  vec3 /= 5;
  assert_vec(vec3, 1);
  vector4i vec4(5);
  vec4 /= 5;
  assert_vec(vec4, 1);
}

void test_dot() {
  assert(vector2i(1, 1).dot(vector2i(1, -1)) == 0);
  assert(vector3i(1, 2, 1).dot(vector3i(-1, 1, -1)) == 0);
  assert(vector4i(1, 3, 3, 1).dot(vector4i(-3, 1, 1, -3)) == 0);
}

void test_size() {
  assert(COMPARE_EQ(vector2i(3, 4).size(), 5.0f));
  assert(COMPARE_EQ(vector3i(1, 0, 0).size(), 1.0f));
  assert(COMPARE_EQ(vector4i().size(), 0.0f));
}

void test_is_zero() {
  assert(vector2i().is_zero());
  assert(vector3i().is_zero());
  assert(vector4i().is_zero());
}

void test_normalized() {
  assert(COMPARE_EQ(vector2f(1, 2).normalized().size(), 1.0f));
  assert(COMPARE_EQ(vector3f(1, 2, 3).normalized().size(), 1.0f));
  assert(COMPARE_EQ(vector4f(1, 2, 3, 4).normalized().size(), 1.0f));
}

int main(int argc, char** argv) {
  std::cout << "> Testing constructors... " << std::flush;
  test_constructors();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing addition... " << std::flush;
  test_add();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing difference... " << std::flush;
  test_minus();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing element-wise multiplication... " << std::flush;
  test_elm_mult();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing element-wise division... " << std::flush;
  test_elm_div();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing scalar multiplication... " << std::flush;
  test_scalar_mult();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing scalar division... " << std::flush; 
  test_scalar_div();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing dot... " << std::flush;
  test_dot();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing size... " << std::flush;
  test_size();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing is_zero... " << std::flush;
  test_is_zero();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Testing normalized... " << std::flush;
  test_normalized();
  std::cout << "PASSED" << std::endl;

  std::cout << "> Congratulations! All tests passed!" << std::endl;
  return 0;
}
