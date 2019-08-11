#include "main.h"
#include <chrono>
#include <iostream>
#include <vector>
#include "ev.h"

int main() {
  // TODO:  move this somewhere else

  auto ev = ev::Ev{};

  ev.main_loop();

  return 0;
}

namespace ev {
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

void main_loop() {}
}  // namespace ev
