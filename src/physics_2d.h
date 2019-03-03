#pragma once
#include <vector>

namespace phys {

struct Vec2f {
  float x;
  float y;

  Vec2f inline operator=(Vec2f a) {
    x = a.x;
    y = a.y;
    return a;
  }

  Vec2f inline operator+(Vec2f a) { return {a.x + x, a.y + y}; }

  const Vec2f inline operator-(Vec2f a) const { return {a.x - x, a.y - y}; }

  Vec2f inline operator-(Vec2f a) { return {a.x - x, a.y - y}; }

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

struct PhysObject {
  Circle circle;
  Vec2f velocity;
  float restitution;  // How bouncy this object is in collisions
  float mass;
};

class Simulator {
 public:
  void add(PhysObject object);
  void step(float dt);
  std::vector<PhysObject> objects();

 private:
  std::vector<PhysObject> m_objects{};
};

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

float SquaredDistance(Vec2f a, Vec2f b);
float Distance(Vec2f a, Vec2f b);

bool AABBvsAABB(AABB a, AABB b);
bool CirclevsCircle(Circle a, Circle b);

Vec2f collisionNormal(Circle A, Circle B);

void ResolveCollision(const PhysObject& A, const PhysObject& B);

}  // namespace phys
