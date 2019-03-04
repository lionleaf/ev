#include "common.h"

Creature::Creature() {
  m_phys_obj.circle = Circle{1.0f, Vec2f{0.0f, -6.0f}};
  m_phys_obj.mass = 2.0f;
  m_phys_obj.restitution = 0.5f;
  m_gene_velocity = Vec2f{1.0f, 0.5f};
  m_time_accumulator = 0.0f;
}

void Creature::update(float dt) {
  m_time_accumulator += dt;
  if (m_time_accumulator > 0.5f) {
    m_phys_obj.velocity += m_gene_velocity;
    m_time_accumulator -= 0.5f;
  }
}