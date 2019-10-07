#include "ev.h"

namespace ev {

void EvBase::render_world(phys::World& world) {
  for (Body* body : world.objects()) {
    m_renderer.draw_body(*body);
  }
}

}  // namespace ev
