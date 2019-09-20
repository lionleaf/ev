#pragma once
#include <math.h>
namespace ev {
using real = double;
struct Vec2 {
  real x;
  real y;

  void inline normalize() {
    double len = sqrt(x * x + y * y);
    if (len == 0.0) {
      x = 0.0f;
      y = 0.0f;
    } else {
      x *= static_cast<real>(1.0f / len);
      y *= static_cast<real>(1.0f / len);
    }
  }

  double inline length_squared() const { return (x * x + y * y); }

  // Returns a copy of the vector with the rotation applied
  Vec2 inline with_rotation(real rotation_radians) const {
    real sn = static_cast<real>(sin(rotation_radians));
    real cs = static_cast<real>(cos(rotation_radians));
    return Vec2{x * cs - y * sn, x * sn + y * cs};
  }

  void inline abs() {
    x = fabs(x);
    y = fabs(y);
  }

  // Rotates vector in-place
  void inline rotate(real rotation_radians) {
    real sn = static_cast<real>(sin(rotation_radians));
    real cs = static_cast<real>(cos(rotation_radians));

    real new_x = x * cs - y * sn;  // old x is used to calculate y, use temp
    y = x * sn + y * cs;
    x = new_x;
  }

  bool operator==(const Vec2& a) { return (x == a.x && y == a.y); }

  Vec2 inline operator=(Vec2 a) {
    x = a.x;
    y = a.y;
    return a;
  }

  inline Vec2& operator*=(const real& a) {
    x *= a;
    y *= a;
    return *this;
  }

  inline Vec2& operator+=(const Vec2& a) {
    x += a.x;
    y += a.y;
    return *this;
  }
  inline Vec2& operator-=(const Vec2& a) {
    x -= a.x;
    y -= a.y;
    return *this;
  }
  real inline operator*(Vec2 a) { return a.x * x + a.y * y; }
};

Vec2 inline operator+(Vec2 lhs, const Vec2& rhs) {
  lhs += rhs;
  return lhs;
}

Vec2 inline operator-(Vec2 lhs, const Vec2& rhs) {
  lhs -= rhs;
  return lhs;
}

Vec2 inline operator-(Vec2 vec) {
  vec.x = -vec.x;
  vec.y = -vec.y;
  return vec;
}

struct AABB {
  Vec2 min;
  Vec2 max;
  AABB(Vec2 min, Vec2 max) : min{min}, max{max} {}
};

real inline squared_distance(Vec2 a, Vec2 b) {
  auto x = (a.x - b.x);
  auto y = (a.y - b.y);
  return x * x + y * y;
}

real inline squared_length(Vec2 a) {
  return a.x * a.x + a.y * a.y;
}

real inline distance(Vec2 a, Vec2 b) {
  real x = (a.x - b.x);
  real y = (a.y - b.y);
  return static_cast<real>(sqrt(x * x + y * y));
}

real inline dot_product(Vec2 a, Vec2 b) {
  return a.x * b.x + a.y * b.y;
}

real inline cross_product(const Vec2& a, const Vec2& b) {
  return a.x * b.y - a.y * b.x;
}

// Note that the result of a 2D scalar cross product is order dependent
auto inline cross_product(const Vec2& a, real s) {
  return Vec2{s * a.y, -s * a.x};
}

auto inline cross_product(real s, const Vec2& a) {
  return Vec2{-s * a.y, s * a.x};
}

Vec2 inline operator*(Vec2 b, real a) {
  return {a * b.x, a * b.y};
}

Vec2 inline operator*(real a, Vec2 b) {
  return {a * b.x, a * b.y};
}

Vec2 inline operator/(Vec2 a, real b) {
  return {a.x / b, a.y / b};
}

Vec2 inline operator/(real a, Vec2 b) {
  return {a / b.x, a / b.y};
}
}  // namespace ev
