#include "ev.h"
#include <algorithm>
#include <chrono>
#include "ev_ui.h"

using namespace std::chrono;

namespace ev {
Ev::Ev() {}

template <typename T>
int inline max_element_index(std::vector<T> vec) {
  return std::max_element(begin(vec), end(vec)) - begin(vec);
}

void Ev::main_loop() {
  high_resolution_clock::time_point last_tick =
      high_resolution_clock::now();  // TODO: Better suited time stamp?

  // TODO: this should be independent, but need to decouple while rendering in
  // that case
  float simulation_dt = static_cast<float>(m_renderer.frame_duration);
  double dt_accumulator =
      1;  // Start above 0 to ensure first frame renders instantly

  constexpr int population_count = 100;
  Generation generation =
      m_evolutor.generate_fresh_generation(population_count);
  std::vector<double> fitness(population_count);

  int simulated_creatures = 0;

  CreatureDNA best_dna{};

  auto challenge = WalkingChallenge{generation.dna[simulated_creatures]};
  std::vector<std::shared_ptr<WalkingChallenge>> visible_challenges{};
  for (CreatureDNA dna : generation.dna) {
    visible_challenges.push_back(std::make_shared<WalkingChallenge>(dna));
    break;
  }

  Camera camera{};
  camera.pos = glm::vec3{0, 0, 50};  // Look down -Z axis.
  camera.look_at = glm::vec3{0, 0, 0};
  camera.up = glm::vec3{0, 1, 0};

  Creature* visible_creature{};
  while (!m_renderer.should_close()) {
    // Keep the invisible simulations running as fast as possible
    if (challenge.step(simulation_dt)) {
      fitness[simulated_creatures] = challenge.get_fitness();

      ++simulated_creatures;
      if (simulated_creatures >= population_count) {
        best_dna = generation.dna[max_element_index(fitness)];
        generation = m_evolutor.breed_next_generation(generation, fitness,
                                                      population_count);
        simulated_creatures = 0;
      }
      // Current simulation done! Switch to next one.
      challenge.reset(generation.dna[simulated_creatures]);
    }

    // This loop is hopefully running significantly faster than the render rate
    high_resolution_clock::time_point now = high_resolution_clock::now();
    double dt =
        duration_cast<nanoseconds>(now - last_tick).count() / 1000000000.0;
    dt_accumulator += dt;
    last_tick = now;

    // Is it time to render a frame?
    if (dt_accumulator > m_renderer.frame_duration) {
      m_renderer.start_frame();

      ev_ui::generation_info(generation.generation_nr, simulated_creatures,
                             m_evolutor.get_max_fitness_plot());
      for (auto& chal : visible_challenges) {
        render_world(chal->getWorld());
      }
      m_renderer.end_frame();

      while (dt_accumulator > m_renderer.frame_duration) {
        dt_accumulator -= m_renderer.frame_duration;
        // Potentially loops multiple times to make
        // sure we skip frames if we're falling behind
      }

      for (uint32 i = 0; i < visible_challenges.size(); ++i) {
        if (visible_challenges[i]->step(simulation_dt) ||
            visible_challenges[i]->step(
                simulation_dt)) {  // Rendering 30fps simulation at 60fps, so
                                   // double-step
          // visible_challenges[i]->reset(generation.dna[i]);
          visible_challenges[i]->reset(best_dna);
        }
      }
    }
  }
}

void Ev::render_world(phys::World& world) {
  for (Body* body : world.objects()) {
    m_renderer.draw_body(*body);
  }
}

}  // namespace ev
