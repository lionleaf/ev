#include "physics_2d.h"
#include <math.h>
#include <cstdlib>
#include <iostream>
#include "common.h"
#include "ev_math.h"

// unsigned int fp_control_state = _controlfp(_EM_INEXACT, _MCW_EM);

inline void component_clamp(const Vec2f& min, const Vec2f& max, Vec2f& vec) {
  // Clamps each component of vec between min and max

  vec.x = vec.x < min.x ? min.x : vec.x;
  vec.x = vec.x > max.x ? max.x : vec.x;

  vec.y = vec.y < min.y ? min.y : vec.y;
  vec.y = vec.y > max.y ? max.y : vec.y;
}

bool AABB_vs_circle(AABB aabb, Circle circle, CollisionData& collision_data) {
  Vec2f aabb_size = aabb.max - aabb.min;
  auto aabb_extent = aabb_size / 2;
  Vec2f pos_aabb = collision_data.body_a.pos + aabb.min +
                   aabb_extent;  // Center of aabb in world space
  Vec2f pos_circle = collision_data.body_b.pos +
                     circle.pos;  // Center of circle in world space
  auto aabb_to_circle = pos_circle - pos_aabb;

  // Compute closest point on the AABB to the circle
  // By clamping the position of the circle center to the AABB, we get the
  // closest point on (or inside) the AABB to the center
  auto closest_to_circle = pos_circle;
  component_clamp(pos_aabb - aabb_extent, pos_aabb + aabb_extent,
                  closest_to_circle);

  // If the point on the AABB closest to the circle is hitting the center of
  // the circle, we know it's inside the AABB and need a special case (flip
  // normal)
  auto inside = false;
  if (aabb_to_circle == closest_to_circle) {
    inside = true;

    if (abs(aabb_to_circle.x) > abs(aabb_to_circle.y)) {
      if (closest_to_circle.x > 0) {
        closest_to_circle.x = aabb_extent.x;
      } else {
        closest_to_circle.x = -1 * aabb_extent.x;
      }
    } else {
      if (closest_to_circle.y > 0) {
        closest_to_circle.y = aabb_extent.y;
      } else {
        closest_to_circle.y = -1 * aabb_extent.y;
      }
    }
  }

  auto normal = pos_circle - closest_to_circle;
  auto dis = squared_length(normal);

  if (dis > circle.radius * circle.radius && !inside) {
    return false;
  }

  dis = sqrt(dis);  // Delay this until after early exit

  if (inside) {
    collision_data.normal = normal;
    collision_data.normal.normalize();
    collision_data.penetration_depth = circle.radius - dis;
  } else {
    collision_data.normal = -1 * normal;
    collision_data.normal.normalize();
    collision_data.penetration_depth = circle.radius - dis;
  }

  return true;
}

bool AABB_vs_AABB(AABB relative_a,
                  AABB relative_b,
                  CollisionData& collision_data) {
  // Create new AABB in world space
  AABB abox = relative_a;
  abox.min += collision_data.body_a.pos;
  abox.max += collision_data.body_a.pos;
  AABB bbox = relative_b;
  bbox.min += collision_data.body_b.pos;
  bbox.max += collision_data.body_b.pos;

  Vec2f a_half_extent = (abox.max - abox.min) / 2.0f;
  Vec2f b_half_extent = (bbox.max - bbox.min) / 2.0f;

  Vec2f a_to_b = (bbox.min + b_half_extent) - (abox.min + a_half_extent);

  //     ----------      -----------------
  //     |        |      |               |
  //     |    .---|------|------>.       |
  //     |        |      |               |
  //     ----------      -----------------

  float x_overlap = a_half_extent.x + b_half_extent.x - abs(a_to_b.x);
  if (x_overlap <= 0) {
    return false;
  }

  float y_overlap = a_half_extent.y + b_half_extent.y - abs(a_to_b.y);
  if (y_overlap <= 0) {
    return false;
  }

  // Which axis has minimum penetration?
  if (x_overlap < y_overlap) {  // Least penetration in X
    collision_data.penetration_depth = x_overlap;
    if (a_to_b.x < 0) {
      collision_data.normal = Vec2f{1, 0};
    } else {
      collision_data.normal = Vec2f{-1, 0};
    }
  } else {  // Least penetration in Y
    collision_data.penetration_depth = y_overlap;
    if (a_to_b.y < 0) {
      collision_data.normal = Vec2f{0, 1};
    } else {
      collision_data.normal = Vec2f{0, -1};
    }
  }
  return true;
}

bool circle_vs_circle(Circle circle_a,
                      Circle circle_b,
                      CollisionData& collision_data) {
  Vec2f a_pos =
      collision_data.body_a.pos + circle_a.pos;  // Positions are additive
  Vec2f b_pos =
      collision_data.body_b.pos + circle_b.pos;  // Positions are additive

  // Test intersection
  float combined_r = circle_a.radius + circle_b.radius;
  combined_r *= combined_r;

  if (combined_r < squared_distance(a_pos, b_pos)) {
    return false;  // No collision, early exit
  }

  // Calculate collision normal
  Vec2f unormalized = a_pos - b_pos;
  float len = sqrt(pow(unormalized.x, 2) + pow(unormalized.y, 2));
  collision_data.normal = Vec2f{unormalized.x / len, unormalized.y / len};

  // Calculate penetration depth used to avoid sinking.
  collision_data.penetration_depth = circle_a.radius + circle_b.radius - len;

  return true;
}

void resolve_collision(CollisionData& collision_data) {
  Body& A = collision_data.body_a;
  Body& B = collision_data.body_b;
  Vec2f normal = collision_data.normal;

  Vec2f relative_velocity = A.velocity - B.velocity;

  float vel_along_normal = dot_product(relative_velocity, normal);

  if (vel_along_normal > 0) {
    return;  // Objects already moving apart
  }

  float restitution = fmin(A.restitution, B.restitution);

  float a_inv_mass = A.mass == 0.0f ? 0.0f : 1.0f / A.mass;  // TODO: Precalc
  float b_inv_mass = B.mass == 0.0f ? 0.0f : 1.0f / B.mass;

  float impulse_magnitude = -(1 + restitution) * vel_along_normal;
  impulse_magnitude /= (a_inv_mass + b_inv_mass);

  Vec2f impulse = impulse_magnitude * normal;

  A.velocity += a_inv_mass * impulse;
  B.velocity -= b_inv_mass * impulse;

  // Avoid sinking

  // No sink correction if it's only a tiny sinkage
  const float epsilon = 0.01f;
  // This can be tweaked to avoid sinking and jittering during rest
  const float correction_factor = 0.2f;

  Vec2f correction = (fmax(collision_data.penetration_depth - epsilon, 0.0f) /
                      (a_inv_mass + b_inv_mass)) *
                     (correction_factor * normal);
  // TODO: How does this work with composite bodies?
  A.pos += a_inv_mass * correction;
  B.pos -= b_inv_mass * correction;

  // Apply friction
  relative_velocity = A.velocity - B.velocity;  // This has changed now
  Vec2f tangent =
      relative_velocity - dot_product(relative_velocity, normal) * normal;
  tangent.normalize();

  float friction_impulse_magnitude =
      dot_product(relative_velocity, tangent) * -1;
  friction_impulse_magnitude /= (a_inv_mass + b_inv_mass);

  float static_friction = 0.5f;   // TODO: Should be material property
  float dynamic_friction = 0.3f;  // TODO: Should be material property

  Vec2f friction_impulse{};

  // Coulomb's Law  (inequality):
  if (abs(friction_impulse_magnitude) < static_friction * impulse_magnitude) {
    friction_impulse = tangent * friction_impulse_magnitude;
  } else {
    friction_impulse = -1 * impulse_magnitude * tangent * dynamic_friction;
  }  // TODO: Research if this is actually a legit way to do friction

  A.velocity += a_inv_mass * friction_impulse;
  B.velocity -= b_inv_mass * friction_impulse;
}

float PhysicsSimulator::walking_challenge(Creature creature,
                                          OpenGLRenderer* optional_renderer,
                                          int seconds,
                                          int nr_bodies) {
  static int number_of_iterations = 60 * seconds;
  static float dt = 1 / 60.0f;
  m_objects.push_back(&PHYS_OBJ_GROUND);
  creature.reset();
  m_objects.push_back(&creature.body());

  srand(3);  // Seed random for deterministic results
  std::vector<Body> body(nr_bodies);
  for (unsigned int i = 0; i < body.size(); i++) {
    if (rand() % 2 == 0) {
      body[i].circles.push_back(Circle{});
      body[i].circles[0].radius = (rand() % 100) / 50.0f + 0.02f;
      body[i].circles[0].pos.x = (rand() % 20 - 10) / 10.0f;
      body[i].circles[0].pos.y = (rand() % 10) / 10.0f;
    } else {
      body[i].rects.push_back(AABB{});
      body[i].rects[0].min.x = (rand() % 20 - 10) / 10.0f;
      body[i].rects[0].min.y = (rand() % 20 - 10) / 10.0f;
      body[i].rects[0].max.x = body[i].rects[0].min.x + (rand() % 20) / 10.0f;
      body[i].rects[0].max.y = body[i].rects[0].min.y + (rand() % 20) / 10.0f;
    }

    body[i].pos.x = (rand() % 200 - 100) / 10.0f;
    body[i].pos.y = (rand() % 100) / 10.0f + i;
    body[i].mass = 10;
    // 3.14f * pow(body[i].circles[0].radius, 3);  // We are used to 3D
    body[i].restitution = 0.5f;
    body[i].velocity.x = (rand() % 100 / 10.f - 5);
    body[i].velocity.y = (rand() % 100 / 10.f - 5);
    m_objects.push_back(&body[i]);
  }

  for (int i = 0; i < number_of_iterations; ++i) {
    step(dt);
    creature.update(dt);
    if (optional_renderer) {
      if (optional_renderer->shouldClose()) {
        return -1.0f;  // Interrupted by user!
      }
      optional_renderer->clear();
      for (Body* body : m_objects) {
        optional_renderer->draw_body(*body);
      }

      optional_renderer->finishRendering();
    }
  }
  m_objects.clear();
  return creature.body().pos.x;
}

void PhysicsSimulator::add(Body* object) {
  m_objects.push_back(object);
}

void PhysicsSimulator::step(float dt) {
  Vec2f gravity{0.0f, -5.5f};
  for (Body* obj : m_objects) {
    if (obj->mass == 0.0f) {
      continue;  // inf mass
    }
    obj->pos = obj->pos + obj->velocity * dt;
    obj->velocity = obj->velocity + gravity * dt;
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

      for (AABB rect_a : obj_a->rects) {
        for (AABB rect_b : obj_b->rects) {
          CollisionData collision_data{*obj_a, *obj_b};
          if (AABB_vs_AABB(rect_a, rect_b, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }

      for (AABB rect : obj_a->rects) {
        for (Circle circle : obj_b->circles) {
          CollisionData collision_data{*obj_a, *obj_b};
          if (AABB_vs_circle(rect, circle, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }

      for (Circle circle : obj_a->circles) {
        for (AABB rect : obj_b->rects) {
          CollisionData collision_data{*obj_b, *obj_a};
          if (AABB_vs_circle(rect, circle, collision_data)) {
            collisions.push_back(std::move(collision_data));
          }
        }
      }
      // TODO: AABB_vs_circle
    }
  }

  for (CollisionData collision : collisions) {
    resolve_collision(collision);
  }
}

std::vector<Body*> PhysicsSimulator::objects() {
  return m_objects;
}
