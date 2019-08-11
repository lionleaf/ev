#pragma once
#include <memory>
#include <vector>
#include "ev_math.h"
#include "shapes.h"

namespace ev {

using uint32 = uint32_t;

template <typename T>
using u = std::unique_ptr<T>;

class Body {
 public:
  std::vector<Polygon> polygons;
  std::vector<Circle> circles;

  // Linear motion
  Vec2f m_pos{};
  Vec2f m_velocity{};
  float acceleration{};

  // Rotational motion
  float m_orientation{};  // radians
  float m_angular_velocity{0.000f};
  float m_torque{};

  float restitution{};  // How bouncy this object is in collisions
  float mass{0.0f};
  float m_moment_of_inertia{1.0f};

  Body() {}
  ~Body();  // Definition in common.cpp to be able to include shapes.h

  Body(Vec2f pos, Circle circle, Polygon polygon);
};

struct CreatureDNA {
  constexpr static int dna_size{5};
  float raw_dna[dna_size];
};

struct Generation {
  int generation_nr{};
  std::vector<CreatureDNA> dna{};
};

class Creature {
 public:
  Creature();
  Creature(const CreatureDNA& dna);
  void reset(){};
  void update(float dt);
  Vec2f m_gene_velocity{};
  Body& body() { return m_body; };

 private:
  Body m_body{};
  float m_time_accumulator{0};
};

}  // namespace ev
