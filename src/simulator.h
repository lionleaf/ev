#pragma once
#include "physics_2d.h"
#include "shapes.h"

namespace ev {

template <class T>
class WalkingChallenge {
 public:
  using CreatureType = T;
  WalkingChallenge(CreatureDNA creatureDNA,
                   int seconds = 15,
                   int nr_bodies = 0);

  // Returns true if challenge is done
  bool step(float dt);

  // Only call after simulation is done (ie. step returns true)
  real get_fitness();

  void reset(CreatureDNA new_creatureDNA);
  phys::World& getWorld();

 private:
  std::unique_ptr<CreatureType> m_creature;
  Body m_ground{{0.0f, -50.0f}, {}, Polygon{5000.0f, 50.0f}, true};
  phys::World m_world;
  int m_num_iterations{0};
  int m_iterations_to_complete{};
  int m_nr_bodies{};
  static constexpr float m_dt = 1.0f / 60.0f;
};

template <class T>
WalkingChallenge<T>::WalkingChallenge(CreatureDNA creatureDNA,
                                      int seconds,
                                      int nr_bodies) {
  m_nr_bodies = nr_bodies;
  m_iterations_to_complete = 60 * seconds;
  m_creature = std::make_unique<CreatureType>(creatureDNA);

  m_world.add(&m_ground);
  m_world.add(&m_creature->body());
  m_world.add_random_bodies(nr_bodies);
}

template <class T>
bool WalkingChallenge<T>::step(float dt) {
  if (m_num_iterations < m_iterations_to_complete) {
    m_creature->step(dt);
    m_world.step(dt);
    ++m_num_iterations;
  }
  return m_num_iterations == m_iterations_to_complete;
}

template <class T>
real WalkingChallenge<T>::get_fitness() {
  assert(m_num_iterations == m_iterations_to_complete);
  return m_creature->body().m_pos.x;
}

template <class T>
void WalkingChallenge<T>::reset(CreatureDNA new_creatureDNA) {
  m_num_iterations = 0;

  m_world.reset();

  m_creature = std::make_unique<CreatureType>(new_creatureDNA);

  m_world.add(&m_ground);
  m_world.add(&m_creature->body());
  m_world.add_random_bodies(m_nr_bodies);
}

template <class T>
phys::World& WalkingChallenge<T>::getWorld() {
  return m_world;
}
}  // namespace ev
