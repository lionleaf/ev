#pragma once
#include <glad/glad.h>
// GLFW must be included after glad.h
#include <GLFW/glfw3.h>
#include <glm/mat4x4.hpp>
#include "common.h"

class OpenGLRenderer {
 public:
  OpenGLRenderer();
  void clear();
  void drawCreature(Creature& creature);
  void draw_body(const Body& body);
  void draw_circle(Circle circle, Vec2f offset);
  void draw_rect(AABB rect, Vec2f offset);
  void finishRendering();

  bool shouldClose();

 private:
  GLFWwindow* mWindow;
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
