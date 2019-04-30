#include "ev.h"
#include <chrono>
#include "ev_ui.h"

using namespace std::chrono;

Ev::Ev() {}

void Ev::main_loop() {
  high_resolution_clock::time_point last_tick =
      high_resolution_clock::now();  // TODO: Better suited time stamp?

  // TODO: this should be independent, but need to decouple while rendering in
  // that case
  float simulation_dt = m_renderer.frame_duration;
  double dt_accumulator = 1; // Start above 0 to ensure first frame renders instantly

  constexpr int population_count = 100;
  Generation generation =
      m_evolutor.generate_fresh_generation(population_count);
  std::vector<double> fitness(population_count);

  int simulated_creatures = 0;

  auto challenge =
      WalkingChallenge{Creature{generation.dna[simulated_creatures]}};
  auto visible_challenge =
      WalkingChallenge{Creature{generation.dna[simulated_creatures]}};
  
  Camera camera{};
  camera.pos = glm::vec3{0, 0, 50};  // Look down -Z axis.
  camera.look_at = glm::vec3{0, 0, 0};
  camera.up = glm::vec3{0, 1, 0};
  
  Vec2f camera_ui_pos{};

  Creature visible_creature{};
  while (!m_renderer.should_close()) {


    // Keep the invisible simulations running as fast as possible
    if (challenge.step(simulation_dt)) {
      fitness[simulated_creatures] = challenge.get_fitness();
      
      ++simulated_creatures;
      if (simulated_creatures >=  population_count) {
        generation = m_evolutor.breed_next_generation(generation, fitness,
                                                      population_count);
        simulated_creatures = 0;
      }
      // Current simulation done! Switch to next one.
      challenge.reset(Creature{generation.dna[simulated_creatures]});
    }
    
    // This loop is hopefully running significantly faster than the render rate
    high_resolution_clock::time_point now = high_resolution_clock::now();
    double dt = duration_cast<nanoseconds>(now - last_tick).count() / 1000000000.0;
    dt_accumulator += dt;
    last_tick = now;
    
    // Is it time to render a frame?
    if (dt_accumulator > m_renderer.frame_duration) {
      
      camera.pos.x = camera_ui_pos.x;
      camera.pos.y = camera_ui_pos.y;
      camera.look_at.x = visible_creature.body().pos.x;
      camera.look_at.y = visible_creature.body().pos.y;
      //camera.look_at.x = camera_ui_pos.x;
      //camera.look_at.y = camera_ui_pos.y;

      m_renderer.start_frame(camera);
      ev_ui::camera_control(camera_ui_pos);
      
      ev_ui::generation_info(generation.generation_nr, simulated_creatures, m_evolutor.get_max_fitness_plot());
      render_world(visible_challenge.getWorld());
      m_renderer.end_frame();
      
      while (dt_accumulator > m_renderer.frame_duration) {
        dt_accumulator -= m_renderer.frame_duration;
        // Potentially loops multiple times to make
        // sure we skip frames if we're falling behind
      }
      
      if(visible_challenge.step(simulation_dt)){
        visible_creature = Creature{generation.dna[simulated_creatures]};
        visible_challenge.reset(visible_creature);
      }
    }
  }
}

void Ev::render_world(phys_2d::World& world) {
  for (Body* body : world.objects()) {
    m_renderer.draw_body(*body);
  }
}
