#pragma once
#include <glad/glad.h>
// GLFW must be included after glad.h
#include <GLFW/glfw3.h>
#include "common.h"

class OpenGLRenderer {
 public:
  OpenGLRenderer();
  void clear();
  void drawCircle(Circle circle);
  void drawCreature(Creature& creature);
  void finishRendering();
  bool shouldClose();

 private:
  GLFWwindow* mWindow;
  GLuint m_shader_program_flat{};
  GLuint m_shader_program_circle{};
};
