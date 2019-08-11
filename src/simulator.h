#pragma once
#include "physics_2d.h"
#include "shapes.h"

namespace ev {
class WalkingChallenge {
 public:
  WalkingChallenge(CreatureDNA creatureDNA,
                   int seconds = 10,
                   int nr_bodies = 20);
  // Returns true if challenge is done
  bool step(float dt);

  // Only call after simulation is done (ie. step returns true)
  float get_fitness();

  void reset(CreatureDNA new_creatureDNA);
  phys::World& getWorld();

 private:
  Creature m_creature;
  Body m_ground{{}, {}, Polygon{300.0f, 1.0f}};
  phys::World m_world;
  int m_num_iterations{0};
  int m_iterations_to_complete{};
  static constexpr float m_dt = 1.0f / 60.0f;
};
}  // namespace ev
