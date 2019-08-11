#include "shapes.h"
#include "common.h"

namespace ev {
Vec2f Polygon::getExtremePoint(Vec2f dir) {
  float best_projection = -std::numeric_limits<float>::infinity();
  Vec2f best_vertex;

  for (uint32 i = 0; i < m_vertices.size(); ++i) {
    Vec2f v = m_vertices[i];
    float projection = dot_product(v, dir);

    if (projection > best_projection) {
      best_vertex = v;
      best_projection = projection;
    }
  }

  return best_vertex;
}

// Must be in a counterclockwise order around (0, 0)
Polygon::Polygon(std::vector<Vec2f> vertices, float rotation_rad) {
  assert(vertices.size() >= 3);
  m_vertices = std::move(vertices);
  compute_face_normals();
}

void Polygon::compute_mass(float density) {
  Vec2f center_of_mass{0.0f, 0.0f};
  float area = 0.0f;
  float moment_of_inertia = 0.0f;

  for (uint32 i_1 = 0; i_1 < m_vertices.size(); ++i_1) {
    // Calculate the area of the triangle made up by a polygon face and (0,0)
    // Note that we are assuming (0, 0) is inside the polygon (an invariant of
    // Polygon)

    uint32 i_2 = (i_1 + 1) % m_vertices.size();  // Wrap around

    Vec2f& p1 = m_vertices[i_1];
    Vec2f& p2 = m_vertices[i_2];

    constexpr float over_3 = 1.0f / 3.0f;

    float D = cross_product(p1, p2);
    float triangle_area = D * 0.5f;

    center_of_mass += triangle_area * over_3 * (p1 + p2);

    float x2 = p1.x * p1.x + p2.x * p1.x + p2.x * p2.x;
    float y2 = p1.y * p1.y + p2.y * p1.y + p2.y * p2.y;

    moment_of_inertia += (0.25f * over_3 * D) * (x2 + y2);

    m_mass = density * area;
    m_moment_of_inertia = moment_of_inertia * density;
  }

  center_of_mass.x *= 1.0f / area;
  center_of_mass.y *= 1.0f / area;

  // Move (0,0) to center of mass
  for (auto& vertex : m_vertices) {
    vertex -= center_of_mass;
  }
}

void Polygon::compute_face_normals() {
  for (uint32 i1 = 0; i1 < m_vertices.size(); ++i1) {
    uint32 i2 = i1 + 1 < m_vertices.size() ? i1 + 1 : 0;
    Vec2f face = m_vertices[i2] - m_vertices[i1];

    // Ensure no zero-length edges, because that's bad
    assert(face.length_squared() > 0.00000000001);

    // Calculate normal with 2D cross product between vector and scalar
    m_normals[i1] = Vec2f{face.y, -face.x};
    m_normals[i1].normalize();
  }
}

Polygon::Polygon(float half_width, float half_height, float rotation_rad) {
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

  m_rotation = rotation_rad;
}
}  // namespace ev
