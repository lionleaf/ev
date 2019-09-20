#pragma once
#include "physics_2d.h"
#include "shapes.h"

namespace ev {
class WalkingChallenge {
 public:
  WalkingChallenge(CreatureDNA creatureDNA,
                   int seconds = 30,
                   int nr_bodies = 20);
  // Returns true if challenge is done
  bool step(float dt);

  // Only call after simulation is done (ie. step returns true)
  real get_fitness();

  void reset(CreatureDNA new_creatureDNA);
  phys::World& getWorld();

 private:
  Creature m_creature;
  Body m_ground{{0.0f, -50.0f}, {}, Polygon{50.0f, 50.0f}, true};
  phys::World m_world;
  int m_num_iterations{0};
  int m_iterations_to_complete{};
  int m_nr_bodies{};
  static constexpr float m_dt = 1.0f / 60.0f;
};
}  // namespace ev
