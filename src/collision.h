#pragma once
#include "common.h"
#include "shapes.h"

namespace ev {
namespace phys {

struct CollisionData {
  Body& body_a;
  Body& body_b;
  Shape& shape_a;
  Shape& shape_b;
  Vec2 normal{};
  real penetration_depth{};
  Vec2 contacts[2]{};  // Only convex polygon and m_circles, so max 2 contact
                       // points
  int contact_count{};
};

void resolve_collision(CollisionData& collision_data);

bool polygon_vs_polygon(Polygon a, Polygon b, CollisionData& collision_data);
bool polygon_vs_circle(Polygon a, Circle b, CollisionData& collision_data);
bool circle_vs_circle(Circle circle_a,
                      Circle circle_b,
                      CollisionData& collision_data);
bool AABB_vs_AABB(AABB relative_a,
                  AABB relative_b,
                  CollisionData& collision_data);
bool AABB_vs_circle(AABB aabb, Circle circle, CollisionData& collision_data);

}  // end namespace phys
}  // end namespace ev
