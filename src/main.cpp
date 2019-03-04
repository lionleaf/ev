#include "main.h"
#include <chrono>
#include <cstdlib>
#include <iostream>
#include <vector>
#include "physics_2d.h"
#include "renderer_opengl.h"

using namespace std;
using namespace std::chrono;

float perfTestFunction() {
  float result = 0;
  Vec2f a{0.5, 200.5};
  Vec2f b{5, 290.5};
  for (int i = 0; i < 100; i++) {
    result += distance(a, b);
  }
  return result;
}

void perf_experiment() {
  // Scratchpad for doing perf experiments
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  perfTestFunction();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << duration;
}

int main() {
  PhysicsSimulator simulator{};
  OpenGLRenderer renderer{};
  Creature test_creature{};

  simulator.walking_challenge(test_creature, &renderer);

  /*// Render loop
  while (!renderer.shouldClose()) {
    // processInput()
    // physics
    simulator.step(1.0f / 60.0f);

    renderer.clear();

    for (PhysObject obj : simulator.objects()) {
      renderer.drawCircle(obj.circle);
    }

    // render
    renderer.finishRendering();
  }*/

  glfwTerminate();
  return 0;
}
