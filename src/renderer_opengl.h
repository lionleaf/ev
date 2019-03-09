#pragma once
#include <glad/glad.h>
// GLFW must be included after glad.h
#include <GLFW/glfw3.h>
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
  GLuint m_shader_program_flat{};
  GLuint m_shader_program_circle{};
};
