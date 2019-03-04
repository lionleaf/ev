#include "evolution.h"

Evolutor::Evolutor() {}

void Evolutor::step_generation() {
  std::cout << "not implemented" << std::endl;
}

void Evolutor::evaluate_generation() {
  std::cout << "not implemented" << std::endl;
}

void Evolutor::breed_new_generation() {}

Creature* Evolutor::get_best_creature() {
  std::cout << "not implemented" << std::endl;
  return nullptr;
}

std::vector<Creature> Evolutor::get_generation(int gen_nr) {
  std::cout << "not implemented" << std::endl;
  return std::vector<Creature>{};
}
