#pragma once
#include <math.h>

struct Vec2f {
  float x;
  float y;

  void inline normalize() {
    float len = sqrt(pow(x, 2) + pow(x, 2));
    if (len == 0.0f) {
      x = 0;
      y = 0;
    } else {
      x = x / len;
      y = y / len;
    }
  }

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
  const Vec2f inline operator-(Vec2f a) const { return {a.x - x, a.y - y}; }
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

float squared_distance(Vec2f a, Vec2f b);
float distance(Vec2f a, Vec2f b);
float dot_product(Vec2f a, Vec2f b);

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
