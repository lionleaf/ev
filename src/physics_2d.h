#pragma once
#include <memory>
#include <vector>
#include "common.h"
#include "renderer_opengl.h"
namespace ev {
namespace phys {

class World {
 public:
  void add(Body* object);
  void add_random_bodies(uint32_t nr);
  void step(float dt);
  void reset();
  std::vector<Body*>& objects();

 private:
  std::vector<std::unique_ptr<Body>> m_owned_objects{};
  std::vector<Body*> m_objects{};
  std::vector<std::unique_ptr<Body>> m_tmp_body_storage{};
};

}  // end namespace phys
}  // namespace ev
