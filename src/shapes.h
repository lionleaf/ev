#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "ev_math.h"
namespace ev {
using std::tuple;

class Body;

class Shape {
 public:
  Vec2 m_pos{};
  Vec2 m_velocity{};
};

class Polygon : public Shape {
 private:
  real m_rotation{};

  void compute_face_normals();

 public:
  Polygon(std::vector<Vec2> vertices, real rotation_rad = 0.0f);
  Polygon(real half_width, real half_height, real rotation_rad = 0.0f);
  void set_rect(real half_width, real half_height);
  Vec2 getExtremePoint(Vec2 dir);
  real inline rotation() const { return m_rotation; }
  Vec2 inline vertex(int index) const { return m_vertices[index]; }
  Vec2 inline normal(int index) const { return m_normals[index]; }
  size_t inline vertex_count() const { return m_vertices.size(); }

  std::vector<Vec2> m_vertices{};
  std::vector<Vec2> m_normals{};

  // Returns {mass, angular_mass} tuple
  tuple<real, real> compute_mass(real density);
};

class Circle : public Shape {
 public:
  real radius;
  real compute_mass(real density);
  real compute_angular_mass(real mass);
};
}  // namespace ev
