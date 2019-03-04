#pragma once
#include <vector>
#include "common.h"
#include "renderer_opengl.h"

static Body PHYS_OBJ_GROUND{Circle{1000.0f, Vec2f{0, -1009.0f}},
                            Vec2f{0.0f, 0.0f}, 0.9f, 0.0f};

class PhysicsSimulator {
 public:
  float walking_challenge(Creature creature, OpenGLRenderer* optional_renderer);
  void add(Body* object);
  void step(float dt);
  std::vector<Body*> objects();

 private:
  std::vector<Body*> m_objects{};
};

bool AABBvsAABB(AABB a, AABB b);
bool CirclevsCircle(Circle a, Circle b);

Vec2f collisionNormal(Circle A, Circle B);

void ResolveCollision(const Body& A, const Body& B);

Vec2f collisionNormal(Circle A, Circle B);
