#pragma once
#include <math.h>
namespace ev {
struct Vec2f {
  float x;
  float y;

  void inline normalize() {
    double len = sqrt(x * x + y * y);
    if (len == 0.0) {
      x = 0.0f;
      y = 0.0f;
    } else {
      x *= static_cast<float>(1.0f / len);
      y *= static_cast<float>(1.0f / len);
    }
  }

  double inline length_squared() { return (x * x + y * y); }

  // Returns a copy of the vector with the rotation applied
  Vec2f inline with_rotation(float rotation_radians) const {
    float sn = static_cast<float>(sin(rotation_radians));
    float cs = static_cast<float>(cos(rotation_radians));
    return Vec2f{x * cs - y * sn, x * sn + y * cs};
  }

  void inline abs() {
    x = fabs(x);
    y = fabs(y);
  }

  // Rotates vector in-place
  void inline rotate(float rotation_radians) {
    float sn = static_cast<float>(sin(rotation_radians));
    float cs = static_cast<float>(cos(rotation_radians));

    float new_x = x * cs - y * sn;  // old x is used to calculate y, use temp
    y = x * sn + y * cs;
    x = new_x;
  }

  bool operator==(const Vec2f& a) { return (x == a.x && y == a.y); }

  Vec2f inline operator=(Vec2f a) {
    x = a.x;
    y = a.y;
    return a;
  }

  inline Vec2f& operator+=(const Vec2f& a) {
    x += a.x;
    y += a.y;
    return *this;
  }
  inline Vec2f& operator-=(const Vec2f& a) {
    x -= a.x;
    y -= a.y;
    return *this;
  }
  float inline operator*(Vec2f a) { return a.x * x + a.y * y; }
};

Vec2f inline operator+(Vec2f lhs, const Vec2f& rhs) {
  lhs += rhs;
  return lhs;
}

Vec2f inline operator-(Vec2f lhs, const Vec2f& rhs) {
  lhs -= rhs;
  return lhs;
}

Vec2f inline operator-(Vec2f vec) {
  vec.x = -vec.x;
  vec.y = -vec.y;
  return vec;
}

struct AABB {
  Vec2f min;
  Vec2f max;
  AABB(Vec2f min, Vec2f max) : min{min}, max{max} {}
};

float inline squared_distance(Vec2f a, Vec2f b) {
  auto x = (a.x - b.x);
  auto y = (a.y - b.y);
  return x * x + y * y;
}

float inline squared_length(Vec2f a) {
  return a.x * a.x + a.y * a.y;
}

float inline distance(Vec2f a, Vec2f b) {
  float x = (a.x - b.x);
  float y = (a.y - b.y);
  return static_cast<float>(sqrt(x * x + y * y));
}

float inline dot_product(Vec2f a, Vec2f b) {
  return a.x * b.x + a.y * b.y;
}

float inline cross_product(const Vec2f& a, const Vec2f& b) {
  return a.x * b.y - a.y * b.x;
}

// Note that the result of a 2D scalar cross product is order dependent
auto inline cross_product(const Vec2f& a, float s) {
  return Vec2f{s * a.y, -s * a.x};
}

auto inline cross_product(float s, const Vec2f& a) {
  return Vec2f{-s * a.y, s * a.x};
}

Vec2f inline operator*(Vec2f b, float a) {
  return {a * b.x, a * b.y};
}

Vec2f inline operator*(float a, Vec2f b) {
  return {a * b.x, a * b.y};
}

Vec2f inline operator/(Vec2f a, float b) {
  return {a.x / b, a.y / b};
}

Vec2f inline operator/(float a, Vec2f b) {
  return {a / b.x, a / b.y};
}
}  // namespace ev
