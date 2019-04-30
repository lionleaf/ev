#pragma once
#include "physics_2d.h"

class WalkingChallenge {
 public:
  WalkingChallenge(Creature creature, int seconds = 10, int nr_bodies = 200);
  // Returns true if challenge is done
  bool step(float dt);

  // Only call after simulation is done (ie. step returns true)
  float get_fitness();

  void reset(Creature new_creature);
  phys_2d::World& getWorld();

 private:
  Creature m_creature;
  phys_2d::World m_world;
  std::vector<Body> m_tmp_bodies;
  int m_num_iterations{0};
  int m_iterations_to_complete{};
  static constexpr float m_dt = 1.0f / 60.0f;
};
