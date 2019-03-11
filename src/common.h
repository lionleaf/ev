#pragma once
#include <vector>
#include "ev_math.h"

struct Body {
  std::vector<Circle> circles{};
  std::vector<AABB> rects{};
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
    Vec2f m_gene_velocity{};
  Body& body() { return m_body; };

 private:
  Body m_body{};
  float m_time_accumulator{0};
};
