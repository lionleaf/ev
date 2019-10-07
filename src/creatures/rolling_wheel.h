#include "../common.h"

namespace ev {
class RollingWheelCreature {
 public:
  RollingWheelCreature();
  RollingWheelCreature(const CreatureDNA& dna);

  void step(real dt);

  static const int m_legs = 8;
  real m_phase[m_legs];
  real m_amplitudes[m_legs];
  real m_freqs[m_legs];

  Body& body() { return m_body; };

 private:
  Body m_body{};
  real m_time{0};
};
}  // namespace ev
