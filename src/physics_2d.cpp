#include "physics_2d.h"
#include <math.h>
#include <cstdlib>
#include <iostream>
#include "collision.h"
#include "common.h"
#include "ev_math.h"
#include "shapes.h"

namespace ev {
namespace phys {
// unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

void World::add(Body* object) {
  m_objects.push_back(object);
}
void World::add_random_bodies(uint32_t nr) {
  for (uint32_t i = 0; i < nr; i++) {
    std::unique_ptr<Body> object = std::make_unique<Body>();
    object->polygons.push_back(Polygon{1.0f, 1.0f});
    object->m_pos = Vec2f{static_cast<float>(rand() % 100),
                          static_cast<float>(rand() % 100)};
    object->mass = 2.0f;
    object->restitution = 0.1f;
    m_objects.push_back(object.get());
    m_owned_objects.push_back(std::move(object));
  }
}

void World::reset() {
  m_objects.clear();
}

void World::step(float dt) {
  Vec2f gravity{0.0f, -9.0f};
  for (Body* obj : m_objects) {
    if (obj->mass == 0.0f) {
      continue;  // inf mass
    }
    obj->m_pos += obj->m_velocity * dt;
    obj->m_orientation += obj->m_angular_velocity;
    obj->m_velocity += gravity * dt;
    obj->m_angular_velocity +=
        obj->m_torque * (1.0f / obj->m_moment_of_inertia) * dt;
  }
  std::vector<CollisionData> collisions{};

  for (unsigned int i = 0; i < m_objects.size(); i++) {
    Body* obj_a = m_objects.at(i);
    for (unsigned int j = i + 1; j < m_objects.size(); j++) {
      Body* obj_b = m_objects.at(j);

      for (Circle circle_a : obj_a->circles) {
        for (Circle circle_b : obj_b->circles) {
          CollisionData collision_data{*obj_a, *obj_b};
          if (circle_vs_circle(circle_a, circle_b, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }

      for (Polygon poly_a : obj_a->polygons) {
        for (Polygon poly_b : obj_b->polygons) {
          CollisionData collision_data{*obj_a, *obj_b};
          if (polygon_vs_polygon(poly_a, poly_b, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }

      for (Polygon polygon : obj_a->polygons) {
        for (Circle circle : obj_b->circles) {
          CollisionData collision_data{*obj_a, *obj_b};
          if (polygon_vs_circle(polygon, circle, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }

      for (Circle circle : obj_a->circles) {
        for (Polygon polygon : obj_b->polygons) {
          CollisionData collision_data{*obj_b, *obj_a};
          if (polygon_vs_circle(polygon, circle, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }
    }
  }

  for (CollisionData collision : collisions) {
    resolve_collision(collision);
  }
}

std::vector<Body*>& World::objects() {
  return m_objects;
}

}  // end namespace phys
}  // namespace ev
