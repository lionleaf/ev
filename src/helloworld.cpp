#include <armadillo>
#include <iostream>
#include "evolution.h"
#include "physics_2d.h"

using namespace arma;

PhysicsSimulator simulator{};
Creature creature{};
OpenGLRenderer renderer{};

vec get_fit(mat generation) {
  vec fit = zeros(generation.n_rows);
  for (int i = 0; i < generation.n_rows; i++) {
    creature.m_gene_velocity.x = (float)generation(i, 0);
    creature.m_gene_velocity.y = (float)generation(i, 1);
    fit(i) = static_cast<double>(
        simulator.walking_challenge(creature, nullptr, 15, 10));
  }
  return fit;
}

int main() {
  int genes = 2;
  int population = 50;
  int iterations = 10;

  mat generation = randu(population, genes);
  vec fit = get_fit(generation);
  //    Sort by best
  uvec I = sort_index(fit, "descend");
  generation = generation.rows(I);

  for (int i = 0; i < iterations; i++) {
    //        Get parents as the top 10 percent.
    int p = 0.1 * generation.n_rows;
    mat parents = generation.rows(1, p);

    //        Mate parents to get the new generation.
    uvec I_parents_1 = randi<uvec>(population, distr_param(0, p));
    uvec I_parents_2 = randi<uvec>(population, distr_param(0, p));

    //        Remove same parent mating.
    uvec I_same = find(I_parents_1 == I_parents_2);
    while (I_same.n_rows) {
      I_parents_2(I_same) = randi<uvec>(I_same.n_rows, distr_param(0, p));
      I_same = find(I_parents_1 == I_parents_2);
    }

    //        Create children from the parents
    generation = generation.rows(I_parents_1);
    for (int j = 0; j < generation.n_rows; j++) {
      int cross_gene = randi<int>(distr_param(0, genes));
      generation.row(j).tail(cross_gene) =
          generation.row(I_parents_2(j)).tail(cross_gene);
    }

    //        Get fit and sort by best.
    fit = get_fit(generation);
    uvec I = sort_index(fit, "descend");
    generation = generation.rows(I);

    //    Look at the best one
    Creature best_creature{};
    best_creature.m_gene_velocity.x = (float)generation(1, 0);
    best_creature.m_gene_velocity.y = (float)generation(1, 1);
    cout << "Best from generation " << i << ":" << fit(0) << endl;
  }

  //    Look at the best one
  while (true) {
    Creature best_creature{};
    best_creature.m_gene_velocity.x = (float)generation(1, 0);
    best_creature.m_gene_velocity.y = (float)generation(1, 1);
    float best_fit =
        simulator.walking_challenge(best_creature, &renderer, 15, 10);
    cout << "Best fit found : " << best_fit << endl;
  }
  return 0;
}
