#define _USE_MATH_DEFINES
#include "shapes.h"
#include <cmath>
#include "common.h"

namespace ev {
Vec2 Polygon::getExtremePoint(Vec2 dir) {
  real best_projection = -std::numeric_limits<real>::infinity();
  Vec2 best_vertex{};

  for (uint32 i = 0; i < m_vertices.size(); ++i) {
    Vec2 v = m_vertices[i];
    real projection = dot_product(v, dir);

    if (projection > best_projection) {
      best_vertex = v;
      best_projection = projection;
    }
  }

  return best_vertex;
}

// Must be in a counterclockwise order around (0, 0)
Polygon::Polygon(std::vector<Vec2> vertices, real rotation_rad) {
  assert(vertices.size() >= 3);
  m_vertices = std::move(vertices);
  compute_face_normals();
}

tuple<real, real> Polygon::compute_mass(real density) {
  Vec2 center_of_mass{0.0f, 0.0f};
  real area = 0.0f;
  real moment_of_inertia = 0.0f;

  for (uint32 i_1 = 0; i_1 < m_vertices.size(); ++i_1) {
    // Calculate the area of the triangle made up by a polygon face and (0,0)
    // Note that we are assuming (0, 0) is inside the polygon (an invariant of
    // Polygon)

    uint32 i_2 = (i_1 + 1) % m_vertices.size();  // Wrap around

    Vec2& p1 = m_vertices[i_1];
    Vec2& p2 = m_vertices[i_2];

    constexpr real over_3 = 1.0f / 3.0f;

    real D = cross_product(p1, p2);
    real triangle_area = D * 0.5f;

    area += triangle_area;

    center_of_mass += triangle_area * over_3 * (p1 + p2);

    real x2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
    real y2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;

    moment_of_inertia += (0.25f * over_3 * D) * (x2 + y2);
  }

  center_of_mass.x *= 1.0f / area;
  center_of_mass.y *= 1.0f / area;

  // Move (0,0) to center of mass
  for (auto& vertex : m_vertices) {
    vertex -= center_of_mass;
  }
  m_pos += center_of_mass;

  moment_of_inertia = moment_of_inertia * density;
  real mass = density * area;
  return {mass, moment_of_inertia};
}

void Polygon::compute_face_normals() {
  for (uint32 i1 = 0; i1 < m_vertices.size(); ++i1) {
    uint32 i2 = i1 + 1 < m_vertices.size() ? i1 + 1 : 0;
    Vec2 face = m_vertices[i2] - m_vertices[i1];

    // Ensure no zero-length edges, because that's bad
    assert(face.length_squared() > 0.00000000001);

    // Calculate normal with 2D cross product between vector and scalar
    m_normals[i1] = Vec2{face.y, -face.x};
    m_normals[i1].normalize();
  }
}

Polygon::Polygon(real half_width, real half_height, real rotation_rad) {
  set_rect(half_width, half_height);
  m_rotation = rotation_rad;
}

void Polygon::set_rect(real half_width, real half_height) {
  //  The m_vertices and m_normals data for a rectangle:
  //    y
  //    ^
  //    |
  //    ---> x          ^
  //                    |
  //                   n[2]
  //        v[3] -------------- v[2]
  //         |                   |
  //  <--n[3]|        (0,0)      | n[1]-->
  //         |                   |
  //        v[0] -------------- v[1]
  //                  n[0]
  //                   |
  //                   V

  m_vertices.resize(4);
  m_vertices[0] = {-half_width, -half_height};
  m_vertices[1] = {half_width, -half_height};
  m_vertices[2] = {half_width, half_height};
  m_vertices[3] = {-half_width, half_height};

  m_normals.resize(4);
  m_normals[0] = {0.0f, -1.0f};
  m_normals[1] = {1.0f, 0.0f};
  m_normals[2] = {0.0f, 1.0f};
  m_normals[3] = {-1.0f, 0.0f};
}

real Circle::compute_mass(real density) {
  // I'll let the mass scale in 3D for more realistic looking physics
  return M_PI * radius * radius * radius * density * 4.0 / 3.0;
}
real Circle::compute_angular_mass(real mass) {
  return mass * radius * radius * 2.0 / 5.0;  // Assuming uniform density
  // TODO: Maybe add mass as if they were 3D objects?
}
}  // namespace ev
