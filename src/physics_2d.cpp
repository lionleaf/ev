#include "physics_2d.h"
#include <math.h>
#include <iostream>
#include "common.h"
#include "ev_math.h"

bool AABBvsAABB(AABB a, AABB b) {
  if (a.max.x < b.min.x || a.min.x > b.max.x)
    return false;
  if (a.max.y < b.min.y || a.min.y > b.max.y)
    return false;
  return true;
}

bool CirclevsCircle(Circle a, Circle b) {
  float combined_r = a.radius + b.radius;
  combined_r *= combined_r;
  return combined_r > squared_distance(a.pos, b.pos);
}

Vec2f collisionNormal(Circle A, Circle B) {
  Vec2f unormalized = A.pos - B.pos;
  float len = sqrt(pow(unormalized.x, 2) + pow(unormalized.y, 2));
  return Vec2f{unormalized.x / len, unormalized.y / len};
}

void ResolveCollision(Body& A, Body& B) {
  Vec2f relative_velocity = A.velocity - B.velocity;
  Vec2f normal = collisionNormal(A.circle, B.circle);

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

  A.velocity = A.velocity + a_inv_mass * impulse;
  B.velocity = B.velocity - b_inv_mass * impulse;

  // Avoid sinking
  float penetration_depth =
      A.circle.radius + B.circle.radius - distance(A.circle.pos, B.circle.pos);
  const float epsilon = 0.01;  // No sink correction if it's only a tiny sinkage
  const float correction_factor =
      0.2f;  // This can be tweaked to avoid sinking and jittering during rest
  Vec2f correction =
      (fmax(penetration_depth - epsilon, 0.0f) / (a_inv_mass + b_inv_mass)) *
      (correction_factor * normal);
  A.circle.pos += a_inv_mass * correction;
  B.circle.pos -= b_inv_mass * correction;
}

float PhysicsSimulator::walking_challenge(Creature creature,
                                          OpenGLRenderer* optional_renderer) {
  static int number_of_iterations = 60 * 10;
  static float dt = 1 / 60.0f;
  m_objects.push_back(&PHYS_OBJ_GROUND);
  creature.reset();
  m_objects.push_back(creature.phys_object());
  for (int i = 0; i < number_of_iterations; ++i) {
    step(dt);
    creature.update(dt);
    if (optional_renderer) {
      if (optional_renderer->shouldClose()) {
        return -1.0f;  // Interrupted by user!
      }
      optional_renderer->clear();
      optional_renderer->drawCircle(PHYS_OBJ_GROUND.circle);
      optional_renderer->drawCreature(creature);
      optional_renderer->finishRendering();
    }
  }
  m_objects.clear();
  return creature.phys_object()->circle.pos.x;
}

void PhysicsSimulator::add(Body* object) {
  m_objects.push_back(object);
}

void PhysicsSimulator::step(float dt) {
  Vec2f gravity{0.0f, -2.5f};
  for (Body* obj : m_objects) {
    if (obj->mass == 0.0f) {
      continue;  // inf mass
    }
    obj->circle.pos = obj->circle.pos + obj->velocity * dt;
    obj->velocity = obj->velocity + gravity * dt;
  }
  for (int i = 0; i < m_objects.size(); i++) {
    Body* obj_a = m_objects.at(i);
    for (int j = i + 1; j < m_objects.size(); j++) {
      Body* obj_b = m_objects.at(j);
      if (CirclevsCircle(obj_a->circle, obj_b->circle)) {
        ResolveCollision(*obj_a, *obj_b);
      }
    }
  }
}

std::vector<Body*> PhysicsSimulator::objects() {
  return m_objects;
}
