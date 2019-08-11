#pragma once
#include <iostream>
#include <vector>
#include "common.h"
#include "physics_2d.h"

namespace ev {
using std::vector;
class Evolutor {
 public:
  Evolutor();
  Generation generate_fresh_generation(const int population);
  Generation breed_next_generation(const Generation generation,
                                   const std::vector<double> fitness,
                                   int population);
  vector<float>& get_max_fitness_plot();

 private:
  vector<float> m_generational_best_fitness{};
};
}  // namespace ev
