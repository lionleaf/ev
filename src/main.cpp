#include "main.h"
#include <chrono>
#include <iostream>
#include <vector>
#include "physics_2d.h"
#include "renderer_opengl.h"

using namespace std;
using namespace std::chrono;
using namespace phys;

float perfTestFunction() {
  float result = 0;
  Vec2f a{0.5, 200.5};
  Vec2f b{5, 290.5};
  for (int i = 0; i < 100; i++) {
    result += Distance(a, b);
  }
  return result;
}

int main() {
  high_resolution_clock::time_point t1 = high_resolution_clock::now();
  float something = perfTestFunction();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  auto duration = duration_cast<microseconds>(t2 - t1).count();

  cout << duration;

  Simulator simulator{};

  simulator.add(PhysObject{
      Circle{1.0, Vec2f{0, 9.0}},
      Vec2f{0, -3},
      0.2,
      1.0,
  });
  simulator.add(PhysObject{
      Circle{2.0, Vec2f{0, -9.0}},
      Vec2f{0, 3},
      0.2,
      1.0,
  });

  OpenGLRenderer renderer{};
  renderer.init();

  // Render loop
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
  }

  glfwTerminate();
  return 0;
}
