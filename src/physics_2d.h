#pragma once
#include <memory>
#include <vector>
#include "common.h"
#include "renderer_opengl.h"

namespace phys_2d {

static Body PHYS_OBJ_GROUND{{},
                            {AABB{{-100.0f, -12.0f}, {100.0f, -10.0f}}},
                            Vec2f{0.0f, 0.0f},
                            0.9f,
                            0.0f};
class World {
 public:
  void add(Body* object);
  void step(float dt);
  void reset();
  std::vector<Body*>& objects();

 private:
  std::vector<Body*> m_objects{};
  std::vector<std::unique_ptr<Body>> m_tmp_body_storage{};
};

struct CollisionData {
  Body& body_a;
  Body& body_b;
  Vec2f normal{};
  float penetration_depth{};
};

bool AABB_vs_circle(const AABB& aabb,
                    const Circle& circle,
                    CollisionData& collision_data);
bool AABB_vs_AABB(AABB a, AABB b, CollisionData& collision_data);
bool circle_vs_circle(Circle a, Circle b, CollisionData& collision_data);

void resolve_collision(CollisionData& collision_data);

}  // end namespace phys_2d
