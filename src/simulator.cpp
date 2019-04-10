#include "simulator.h"

WalkingChallenge::WalkingChallenge(Creature creature,
                                   int seconds,
                                   int nr_bodies) {
  m_iterations_to_complete = 60 * seconds;
  m_creature = creature;

  m_world.add(&phys_2d::PHYS_OBJ_GROUND);
  m_world.add(&m_creature.body());
}

bool WalkingChallenge::step(float dt) {
  if (m_num_iterations < m_iterations_to_complete) {
    m_creature.update(dt);
    m_world.step(dt);
    ++m_num_iterations;
  }
  return m_num_iterations == m_iterations_to_complete;
}

float WalkingChallenge::get_fitness() {
  assert(m_num_iterations == m_iterations_to_complete);
  return m_creature.body().pos.x;
}

void WalkingChallenge::reset(Creature new_creature) {
  m_num_iterations = 0;
  m_creature = new_creature;

  m_world.reset();

  m_world.add(&phys_2d::PHYS_OBJ_GROUND);
  m_world.add(&m_creature.body());
}

phys_2d::World& WalkingChallenge::getWorld() {
  return m_world;
}
