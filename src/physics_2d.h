#pragma once
#include <vector>
#include "common.h"
#include "renderer_opengl.h"

static Body PHYS_OBJ_GROUND{{},
                            {AABB{{-100.0f, -12.0f}, {100.0f, -10.0f}}},
                            Vec2f{0.0f, 0.0f},
                            0.9f,
                            0.0f};

struct CollisionData {
  Body& body_a;
  Body& body_b;
  Vec2f normal{};
  float penetration_depth{};
};

class PhysicsSimulator {
 public:
  float walking_challenge(Creature creature,
                          OpenGLRenderer* optional_renderer = nullptr,
                          int seconds = 50,
                          int nr_bodies = 200);
  void add(Body* object);
  void step(float dt);
  std::vector<Body*> objects();

 private:
  std::vector<Body*> m_objects{};
};

bool AABB_vs_circle(const AABB& aabb,
                    const Circle& circle,
                    CollisionData& collision_data);
bool AABB_vs_AABB(AABB a, AABB b, CollisionData& collision_data);
bool circle_vs_circle(Circle a, Circle b, CollisionData& collision_data);

void resolve_collision(const CollisionData& collision_data);
