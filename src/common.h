#pragma once
#include "ev_math.h"

enum class ShapeType { Circle, Rectangle };

class Shape {
 private:
  ShapeType m_shape_type;
};

struct Body {
  Circle circle;
  Vec2f velocity;
  float restitution;  // How bouncy this object is in collisions
  float mass;
  Shape shape{};
};

class Creature {
 public:
  Creature();
  void reset(){};
  void update(float dt);
  Body* phys_object() { return &m_phys_obj; };

 private:
  Body m_phys_obj{};
  Vec2f m_gene_velocity{};
  float m_time_accumulator{0};
};
