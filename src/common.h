#pragma once
#include <memory>
#include <vector>
#include "ev_math.h"
#include "shapes.h"

namespace ev {

using uint32 = uint32_t;
using int32 = int32_t;

template <typename T>
using u = std::unique_ptr<T>;

using std::vector;

class Body {
 public:
  vector<Polygon> m_polygons;
  vector<Circle> m_circles;

  // Linear motion
  Vec2 m_pos{};
  Vec2 m_velocity{};
  real acceleration{};

  // Rotational motion
  real m_orientation{};  // radians
  real m_angular_velocity{0.000f};
  real m_torque{};

  real restitution{0.2f};  // How bouncy this object is in collisions

  Body() {}
  ~Body();  // Definition in common.cpp to be able to include shapes.h

  Body(Vec2 pos, Circle circle, Polygon polygon, bool fixed = false);

  void add_circle(Circle circle);
  void add_polygon(Polygon polygon);

  void inline apply_impulse(Vec2 impulse, Vec2 contact_vector) {
    m_velocity += m_mass_inv * impulse;
    m_angular_velocity +=
        m_angular_mass_inv * cross_product(contact_vector, impulse);
  }

  void step(real dt);
  void compute_mass();
  real inline mass() { return m_mass; }
  real inline mass_inv() { return m_mass_inv; }
  void inline set_mass(real mass) {
    m_mass = mass;
    if (abs(mass) > 0.00001f) {
      m_mass_inv = 1.0f / mass;
    } else {
      m_mass_inv = 0.0f;
    }
  }

  real inline angular_mass() { return m_angular_mass; }
  real inline angular_mass_inv() { return m_angular_mass_inv; }
  void inline set_angular_mass(real mass) {
    m_angular_mass = mass;
    if (abs(mass) > 0.00001f) {
      m_angular_mass_inv = 1.0f / mass;
    } else {
      m_angular_mass_inv = 0.0f;
    }
  }

 private:
  real m_mass{0.0f};
  real m_mass_inv{0.0f};
  real m_angular_mass{0.0f};
  real m_angular_mass_inv{0.0f};
};

struct CreatureDNA {
  constexpr static int dna_size{6 * 8};
  real raw_dna[dna_size];
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
  void step(real dt);

  static const int m_legs = 8;
  float m_phase[m_legs];
  float m_amplitudes[m_legs];
  float m_freqs[m_legs];

  Body& body() { return m_body; };

 private:
  Body m_body{};
  real m_time{0};
};

}  // namespace ev
