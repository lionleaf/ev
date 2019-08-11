#include "evolution.h"
#include <armadillo>

using namespace arma;
namespace ev {
Evolutor::Evolutor() {}

Generation Evolutor::generate_fresh_generation(const int population) {
  auto generation = Generation{};

  for (int i = 0; i < population; ++i) {
    auto dna = CreatureDNA{};
    for (int j = 0; j < dna.dna_size; ++j) {
      dna.raw_dna[j] = static_cast<float>(rand() % 1000000) / 1000000.0f;
    }
    generation.dna.push_back(std::move(dna));
  }

  return generation;
}

Generation Evolutor::breed_next_generation(const Generation generation,
                                           const std::vector<double> fitness,
                                           int population) {
  auto next_generation = Generation{};
  next_generation.generation_nr = generation.generation_nr + 1;

  arma::uvec sorted_indices = arma::sort_index(arma::vec(fitness), "descend");

  auto max_fitness_last_gen = fitness[sorted_indices[0]];
  m_generational_best_fitness.push_back(
      static_cast<float>(max_fitness_last_gen));

  for (int i = 0; i < population / 5; i++) {
    next_generation.dna.push_back(generation.dna[sorted_indices.at(i)]);
  }

  // Get parents from the top 10 percent.
  auto mom_indices =
      randi<uvec>(population, distr_param(0.0, population * 0.1));
  auto dad_indices =
      randi<uvec>(population, distr_param(0.0, population * 0.1));

  for (int i = 0; i < population; i++) {
    CreatureDNA mom = generation.dna[sorted_indices.at(mom_indices[i])];
    CreatureDNA dad = generation.dna[sorted_indices.at(dad_indices[i])];
    CreatureDNA kid{};
    for (int dna_i = 0; dna_i < kid.dna_size; ++dna_i) {
      // Randomly pick dna from parent
      kid.raw_dna[dna_i] = rand() % 2 ? mom.raw_dna[dna_i] : dad.raw_dna[dna_i];

      if (rand() % 100 < 5) {
        // Mutation!
        kid.raw_dna[dna_i] +=
            ((rand() % 1000000) / 1000000.0f) - 0.5f;  // Between -0.5 and 0.5
      }
    }
    next_generation.dna.push_back(std::move(kid));
  }

  return next_generation;
}

std::vector<float>& Evolutor::get_max_fitness_plot() {
  return m_generational_best_fitness;
}
}  // namespace ev
