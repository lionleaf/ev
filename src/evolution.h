#pragma once
#include <iostream>
#include <vector>
#include "common.h"
#include "physics_2d.h"

struct Generation {
  int generation_id{};
  std::vector<Creature> creatures{};
};

class Evolutor {
 public:
  Evolutor();
  void step_generation();
  void evaluate_generation();
  void breed_new_generation();
  Creature* get_best_creature();
  std::vector<Creature> get_generation(int gen_nr);

 private:
  std::vector<Generation> m_generations{};
  PhysicsSimulator m_physics_simulator{};
};
