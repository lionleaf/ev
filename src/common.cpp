#include "common.h"

Creature::Creature() {
  m_body.circles.push_back(Circle{1.0f, Vec2f{0.0f, 0.0f}});
  m_body.rects.push_back(AABB{{2.0f, 2.0f}, {3.0f, 3.0f}});
  m_body.mass = 2.0f;
  m_body.restitution = 0.1f;
  m_gene_velocity = Vec2f{0.0f, 0.0f};
  m_time_accumulator = 0.0f;
}

void Creature::update(float dt) {
  m_time_accumulator += dt;
  if (m_time_accumulator > 0.5f) {
    m_body.velocity += m_gene_velocity;
    m_time_accumulator -= 0.5f;
  }
}
