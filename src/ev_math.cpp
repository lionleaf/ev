#include "ev_math.h"
#include <math.h>

float squared_distance(Vec2f a, Vec2f b) {
  return pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0);
}

float squared_length(Vec2f a) {
  return pow(a.x, 2.0) + pow(a.y, 2.0);
}

float distance(Vec2f a, Vec2f b) {
  return sqrt(pow((a.x - b.x), 2.0) + pow((a.y - b.y), 2.0));
}

float dot_product(Vec2f a, Vec2f b) {
  return a.x * b.x + a.y * b.y;
}
