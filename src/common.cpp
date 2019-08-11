#include "common.h"
#include "shapes.h"
namespace ev {

Creature::Creature() {
  m_body.circles.push_back(Circle{0.5f, Vec2f{0.0f, 0.0f}});
  m_body.polygons.push_back(Polygon{0.5f, 0.5f});
  m_body.m_angular_velocity = -0.01f;
  m_body.m_orientation = -3.54f / 4.0f;
  m_body.mass = 2.0f;
  m_body.restitution = 0.1f;
  m_gene_velocity = Vec2f{0.0f, 0.0f};
  m_time_accumulator = 0.0f;
  m_body.m_pos.y = 1.5f;
}

Creature::Creature(const CreatureDNA& dna) : Creature() {
  m_gene_velocity = Vec2f{dna.raw_dna[0], dna.raw_dna[1]};
  m_body.m_velocity = m_gene_velocity;
}

void Creature::update(float dt) {
  m_time_accumulator += dt;
  if (m_time_accumulator > 0.5f) {
    m_body.m_velocity += m_gene_velocity;
    m_time_accumulator -= 0.5f;
  }
}
Body::~Body() = default;
Body::Body(Vec2f pos, Circle circle, Polygon polygon) : m_pos{pos} {
  circles.push_back(std::move(circle));
  polygons.push_back(std::move(polygon));
}
}  // namespace ev
