#pragma once
#include <iostream>
#include <vector>
#include "common.h"
#include "physics_2d.h"

class Evolutor {
 public:
  Evolutor();
  Generation generate_fresh_generation(const int population);
  Generation breed_next_generation(const Generation generation,
                                   const std::vector<double> fitness,
                                   int population);

 private:
};
