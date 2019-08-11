#pragma once
#include "common.h"
#include "evolution.h"
#include "renderer_opengl.h"
#include "simulator.h"

namespace ev {
class Ev {
 public:
  Ev();
  void main_loop();

 private:
  // generations (Do we want to keep it all?)
  // stats per generation (dem plots)
  // Simulator
  // Renderer

  Evolutor m_evolutor{};
  OpenGLRenderer m_renderer{};

  void render_world(phys::World& world);
};
}  // namespace ev
