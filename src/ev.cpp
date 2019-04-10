#include "ev.h"
#include <chrono>
#include "ev_ui.h"

using namespace std::chrono;

Ev::Ev() {}

void Ev::main_loop() {
  high_resolution_clock::time_point last_frame =
      high_resolution_clock::now();  // TODO: Better suited time stamp?

  // TODO: this should be independent, but need to decouple while rendering in
  // that case
  float simulation_dt = m_renderer.frame_duration;
  double dt_accumulator = 0;

  constexpr int population_count = 100;
  Generation generation =
      m_evolutor.generate_fresh_generation(population_count);
  std::vector<double> fitness(population_count);
  std::vector<float> ffitness(population_count); //TODO: yea, no.

  int simulated_creatures = 0;

  auto challenge =
      WalkingChallenge{Creature{generation.dna[simulated_creatures]}};
  auto visible_challenge =
      WalkingChallenge{Creature{generation.dna[simulated_creatures]}};

  while (!m_renderer.should_close()) {
    // This loop is hopefully running significantly faster than the render rate
    high_resolution_clock::time_point now = high_resolution_clock::now();
    float dt = duration_cast<milliseconds>(now - last_frame).count() / 1000.0f;
    dt_accumulator += dt;

    // Is it time to render a frame?
    if (dt_accumulator > m_renderer.frame_duration) {
      while (dt_accumulator > m_renderer.frame_duration) {
        dt_accumulator -= m_renderer.frame_duration;
        // Potentially loops multiple times to make
        // sure we skip frames if we're falling behind
      }
      last_frame = now;
      if(visible_challenge.step(dt)){
        visible_challenge.reset(Creature{generation.dna[simulated_creatures]});
      }
      m_renderer.start_frame();
      ev_ui::generation_info(generation.generation_nr, simulated_creatures, ffitness);
      render_world(visible_challenge.getWorld());
      m_renderer.end_frame();

    }

    // Keep the invisible simulations running as fast as possible
    if (challenge.step(simulation_dt)) {
      fitness[simulated_creatures] = challenge.get_fitness();
      ffitness[simulated_creatures] = challenge.get_fitness();

      ++simulated_creatures;
      if (simulated_creatures > population_count) {
        generation = m_evolutor.breed_next_generation(generation, fitness,
                                                      population_count);
        simulated_creatures = 0;
        visible_challenge.reset(Creature{generation.dna[0]});
      }
      // Current simulation done! Switch to next one.
      challenge.reset(Creature{generation.dna[simulated_creatures]});
    }
  }
}

void Ev::render_world(phys_2d::World& world) {
  for (Body* body : world.objects()) {
    m_renderer.draw_body(*body);
  }
}
