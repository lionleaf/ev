#pragma once
#include <vector>
#include "ev_math.h"

struct Body {
  std::vector<Circle> circles{};
  Vec2f velocity{};
  float restitution{};  // How bouncy this object is in collisions
  float mass{};
  Vec2f pos{};
};

class Creature {
 public:
  Creature();
  void reset(){};
  void update(float dt);
  Body& body() { return m_body; };

 private:
  Body m_body{};
  Vec2f m_gene_velocity{};
  float m_time_accumulator{0};
};
