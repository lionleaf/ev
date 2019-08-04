#pragma once
#include <math.h>

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

struct AABB {
  Vec2f min;
  Vec2f max;
};

struct Circle {
  float radius;
  Vec2f pos;
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
