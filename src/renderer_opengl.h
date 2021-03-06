#pragma once
#define IMGUI_IMPL_OPENGL_LOADER_GLAD
#include <glad/glad.h>
// GLFW must be included after glad.h
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "common.h"
namespace ev {
struct Camera {
  glm::vec3 pos{};
  glm::vec3 look_at{};
  glm::vec3 up{0, 1, 0};  // Positive Y is up by default
};

class OpenGLRenderer {
 public:
  static constexpr double frame_duration = 1.0 / 30.0;
  OpenGLRenderer();
  ~OpenGLRenderer();
  void start_frame();
  void end_frame();
  void draw_body(const Body& body);
  void draw_circle(Circle circle, Vec2 offset, float rotation);
  void draw_polygon(const Polygon& polygon, Vec2 offset, float rotation);

  void scroll_callback(GLFWwindow* window, float offset);

  bool should_close();

 private:
  void poll_camera_input();

  GLFWwindow* m_window;
  Camera m_camera{};

  int m_window_width{500};
  int m_window_height{500};

  GLuint m_flat_shader_program{};
  GLuint m_flat_transform_loc{};

  GLuint m_circle_shader_program{};
  GLuint m_circle_transform_loc{};

  glm::mat4 m_view_matrix{};
  glm::mat4 m_projection_matrix{};
  GLuint m_quad_vao{};

  GLuint setup_quad();
};
}  // namespace ev
