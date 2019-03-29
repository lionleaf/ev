#include "main.h"
#include <chrono>
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
  cout << (duration / 1000.0f) << endl;
}

int main() {
  cout << "sizeof " << sizeof(AABB);
  PhysicsSimulator simulator{};
  OpenGLRenderer renderer{};
  Creature test_creature{};

  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  for (int i = 0; i < 100; i++) {
    simulator.walking_challenge(test_creature, nullptr, 10, 30);
  }
  high_resolution_clock::time_point t2 = high_resolution_clock::now();
  auto duration = duration_cast<microseconds>(t2 - t1).count();
  cout << (duration / 1000.0f) << " ms" << endl;

  simulator.walking_challenge(test_creature, &renderer, 20, 30);

  glfwTerminate();
  return 0;
}
