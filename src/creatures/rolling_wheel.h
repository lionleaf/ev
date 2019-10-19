#include <array>
#include "../common.h"

namespace ev {
class RollingWheelCreature {
 public:
  RollingWheelCreature();
  RollingWheelCreature(const CreatureDNA& dna);

  void step(real dt);

  static constexpr int m_legs = 8;
  std::array<real, m_legs> m_phase{0};
  std::array<real, m_legs> m_amplitudes{0};
  std::array<real, m_legs> m_freqs{0};

  Body& body() { return m_body; };

 private:
  Body m_body{};
  real m_time{0};
};
}  // namespace ev
