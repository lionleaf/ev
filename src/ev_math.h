#pragma once
#include <math.h>

struct Vec2f {
  float x;
  float y;

  void inline normalize() {
    float len = sqrt(x * x + y * y);
    if (len == 0.0f) {
      x = 0.0f;
      y = 0.0f;
    } else {
      x *= 1.0f / len;
      y *= 1.0f / len;
    }
  }

  bool operator==(const Vec2f& a) { return (x == a.x && y == a.y); }

  Vec2f inline operator=(Vec2f a) {
    x = a.x;
    y = a.y;
    return a;
  }

  Vec2f inline operator+(Vec2f a) { return {a.x + x, a.y + y}; }
  inline Vec2f& operator+=(Vec2f a) {
    x += a.x;
    y += a.y;
    return *this;
  }
  const Vec2f inline operator-(Vec2f a) const { return {x - a.x, y - a.y}; }
  inline Vec2f& operator-=(Vec2f a) {
    x -= a.x;
    y -= a.y;
    return *this;
  }
  Vec2f inline operator-(Vec2f a) { return {x - a.x, y - a.y}; }
  float inline operator*(Vec2f a) { return a.x * x + a.y * y; }
};

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
  auto x = (a.x - b.x);
  auto y = (a.y - b.y);
  return sqrt(x * x + y * y);
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
