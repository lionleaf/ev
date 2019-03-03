#include "main.h"
#include <chrono>
#include <cstdlib>
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
  perfTestFunction();
  high_resolution_clock::time_point t2 = high_resolution_clock::now();

  auto duration = duration_cast<microseconds>(t2 - t1).count();

  cout << duration;

  Simulator simulator{};
  //Add ground
  simulator.add(PhysObject{Circle{1000.0f,Vec2f{0, -1009.0f}}, Vec2f{0.0f,0.0f}, 0.9f, 0.0f});
  
  srand(1337);  // seed random
  for (int i = 0; i < 30; i++) {
    float radius = (rand() % 20) / 10.0f;
    float restitution = 0.3f;

    Vec2f pos{static_cast<float>(rand() % 100 - 50),
              static_cast<float>(rand() % 100 - 50)};
    Vec2f vel{};
    float mass = radius * radius * radius;  // We are used to 3D masses.

    simulator.add(PhysObject{Circle{radius, pos}, vel, restitution, mass});
  }
  
  

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
