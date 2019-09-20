#include "simulator.h"

namespace ev {

WalkingChallenge::WalkingChallenge(CreatureDNA creatureDNA,
                                   int seconds,
                                   int nr_bodies) {
  m_nr_bodies = nr_bodies;
  m_iterations_to_complete = 30 * seconds;
  m_creature = Creature{creatureDNA};

  m_world.add(&m_ground);
  m_world.add(&m_creature.body());
  m_world.add_random_bodies(nr_bodies);
}

bool WalkingChallenge::step(float dt) {
  if (m_num_iterations < m_iterations_to_complete) {
    m_creature.update(dt);
    m_world.step(dt);
    ++m_num_iterations;
  }
  return m_num_iterations == m_iterations_to_complete;
}

real WalkingChallenge::get_fitness() {
  assert(m_num_iterations == m_iterations_to_complete);
  return -m_creature.body().m_pos.x;
}

void WalkingChallenge::reset(CreatureDNA new_creatureDNA) {
  m_num_iterations = 0;
  m_creature = Creature(new_creatureDNA);

  m_world.reset();

  m_world.add(&m_ground);
  m_world.add(&m_creature.body());
  m_world.add_random_bodies(m_nr_bodies);
}

phys::World& WalkingChallenge::getWorld() {
  return m_world;
}
}  // namespace ev
