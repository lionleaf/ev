#include "main.h"
#include <chrono>
#include <iostream>
#include <vector>
#include <cstdlib>
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

  srand(1337); //seed random
  for(int i = 0; i < 50; i++){
      simulator.add(PhysObject{
          Circle{1.0, 
          Vec2f{static_cast<float>(rand()%20 - 10), static_cast<float>(rand()%20 - 10)}},
          Vec2f{(rand()%10)/2.0f,static_cast<float>(rand()%10)/2.0f},
          0.2,
          1.0,
      });
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
