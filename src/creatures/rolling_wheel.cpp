#include "rolling_wheel.h"
namespace ev {
RollingWheelCreature::RollingWheelCreature()
    : m_amplitudes{}, m_freqs{}, m_phase{} {
  for (int i = 0; i < m_legs; ++i) {
    m_body.add_polygon(Polygon{5.5f, 0.5f, i * 0.785398});
  }
  m_body.m_angular_velocity = 0.00f;
  m_body.m_orientation = 0.0;
  m_body.restitution = 0.2f;
  m_body.m_pos.y = 5.0f;
}

RollingWheelCreature::RollingWheelCreature(const CreatureDNA& dna)
    : RollingWheelCreature() {
  for (int i = 0; i < m_legs; ++i) {
    m_amplitudes[i] = dna.raw_dna[3 * i + 0];
    m_freqs[i] = 0.9f;
    m_phase[i] = 3.14f * dna.raw_dna[3 * i + 2];

    auto dir = Vec2{1.0f, 0.0};
    dir.rotate(m_body.m_polygons[i].rotation());

    m_body.m_polygons[i].m_pos =
        dir * m_amplitudes[i] * sin(m_freqs[i] * m_time + m_phase[i]);
  }
}

void RollingWheelCreature::step(real dt) {
  m_time += dt;

  for (int i = 0; i < m_legs; ++i) {
    Vec2 dir = Vec2{1.0f, 0.0};
    dir.rotate(m_body.m_polygons[i].rotation());

    // Analytical derivate of the position
    m_body.m_polygons[i].m_velocity = dir * m_amplitudes[i] * m_freqs[i] *
                                      cos(m_freqs[i] * m_time + m_phase[i]);
  }
  m_body.compute_mass();
}
}  // namespace ev
