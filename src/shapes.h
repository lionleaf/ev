#pragma once
#include <glm/gtc/matrix_transform.hpp>
#include <glm/mat4x4.hpp>
#include <vector>
#include "ev_math.h"
namespace ev {

class Body;

class Polygon {
 private:
  float m_rotation{};
  float m_mass{};
  float m_moment_of_inertia{};

  void compute_mass(float density);
  void compute_face_normals();

 public:
  Polygon(std::vector<Vec2f> vertices, float rotation_rad = 0.0f);
  Polygon(float half_width, float half_height, float rotation_rad = 0.0f);
  Vec2f getExtremePoint(Vec2f dir);
  float inline mass() const { return m_mass; }
  float inline rotation() const { return m_rotation; }
  Vec2f inline vertex(int index) const { return m_vertices[index]; }
  Vec2f inline normal(int index) const { return m_normals[index]; }
  size_t inline vertex_count() const { return m_vertices.size(); }

  std::vector<Vec2f> m_vertices{};
  std::vector<Vec2f> m_normals{};
  Vec2f m_pos{};
  Body* m_body;
};

class Circle {
 public:
  float radius;
  Vec2f pos;
};
}  // namespace ev
