#include "common.h"
#include "shapes.h"
namespace ev {

Creature::Creature() {
  for (int i = 0; i < 4; ++i) {
    m_body.add_polygon(Polygon{5.5f, 0.5f, i * 0.785398f});
  }
  m_body.m_angular_velocity = 0.00f;
  m_body.m_orientation = -0.1;
  m_body.restitution = 0.2f;
  m_time = 0.0f;
  m_body.m_pos.y = 5.0f;
}

Creature::Creature(const CreatureDNA& dna) : Creature() {
  for (int i = 0; i < 4; ++i) {
    m_amplitudes[i] = 0.05f * dna.raw_dna[3 * i + 0];
    m_freqs[i] = 20.0f * dna.raw_dna[3 * i + 1];
    m_phase[i] = 3.14f * dna.raw_dna[3 * i + 2];

    Vec2 dir = Vec2{1.0f, 0.0};
    dir.rotate(m_body.m_polygons[i].rotation());

    m_body.m_polygons[i].m_pos =
        dir * m_amplitudes[i] * sin(m_freqs[i] * m_time + m_phase[i]);
  }
}

void Creature::step(real dt) {
  m_time += dt;

  for (int i = 0; i < 4; ++i) {
    //    m_body.m_polygons[i].set_rect(
    //        5.5f + m_amplitudes[i] * sin(m_freqs[i] * m_time + m_phase[i]),
    //        0.5f);

    Vec2 dir = Vec2{1.0f, 0.0};
    dir.rotate(m_body.m_polygons[i].rotation());

    m_body.m_polygons[i].m_velocity = dir * m_amplitudes[i] * m_freqs[i] *
                                      cos(m_freqs[i] * m_time + m_phase[i]);
  }
  m_body.compute_mass();
}  // namespace ev
Body::~Body() = default;
Body::Body(Vec2 pos, Circle circle, Polygon polygon, bool fixed /*=false*/)
    : m_pos{pos} {
  m_circles.push_back(circle);
  m_polygons.push_back(polygon);
  if (fixed) {  // For fixed bodies, mass should stay at 0
    m_mass = 0;
    m_mass_inv = 0;
  } else {
    compute_mass();
  }
}

void Body::add_circle(Circle circle) {
  m_circles.push_back(circle);
  compute_mass();
}

void Body::add_polygon(Polygon polygon) {
  m_polygons.push_back(polygon);
  compute_mass();
}

void Body::step(real dt) {
  if (m_mass == 0.0f) {
    return;  // inf mass
  }
  Vec2 gravity{0.0f, -9.0f};
  m_pos += m_velocity * dt;
  m_orientation += m_angular_velocity * dt;
  m_velocity += gravity * dt;
  m_angular_velocity += m_torque * angular_mass_inv() * dt;
  for (Polygon& poly : m_polygons) {
    poly.m_pos += poly.m_velocity;
  }
}

void Body::compute_mass() {
  struct ShapeProperty {
    Vec2 pos;
    real mass;
    real angular_mass;
  };

  real mass = 0.0;
  real angular_mass = 0.0;
  Vec2 center_of_mass{0.0};

  vector<ShapeProperty> shapes{};

  for (Circle& circle : m_circles) {
    real curr_mass = circle.compute_mass(1.0);
    real curr_angular_mass = circle.compute_angular_mass(curr_mass);

    mass += curr_mass;
    center_of_mass += curr_mass * circle.m_pos;

    shapes.push_back({circle.m_pos, curr_mass, curr_angular_mass});
  }

  for (Polygon& polygon : m_polygons) {
    real curr_mass, curr_angular_mass = 0.0;
    std::tie(curr_mass, curr_angular_mass) = polygon.compute_mass(1.0);
    mass += curr_mass;
    center_of_mass += curr_mass * polygon.m_pos;

    shapes.push_back({polygon.m_pos, curr_mass, curr_angular_mass});
  }

  set_mass(mass);  // Sets m_mass_inv as well.

  center_of_mass *= m_mass_inv;
  // Move all the objects so center of mass is (0,0)

  for (Circle& circle : m_circles) {
    circle.m_pos -= center_of_mass;
  }

  for (Polygon& poly : m_polygons) {
    poly.m_pos -= center_of_mass;
  }
  m_pos += center_of_mass;
  center_of_mass = {0, 0};  // CoM is now at the local origin

  // Sum up angular mass, using the parallel axis theorem
  // And the fact that the center of mass is now the local origin
  // so distance between center of masses is just shape.pos.length
  for (const auto& shape : shapes) {
    angular_mass +=
        shape.angular_mass + shape.mass * shape.pos.length_squared();
  }
  set_angular_mass(angular_mass);
}
}  // namespace ev
