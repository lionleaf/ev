#include "renderer_opengl.h"
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
#include "utils_opengl.h"

using namespace phys;

void OpenGLRenderer::init() {
  // GLFW init
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Needed for Mac

  // Window creation
  mWindow = glfwCreateWindow(800, 600, "EvoView", nullptr, nullptr);
  if (mWindow == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  glfwMakeContextCurrent(mWindow);

  // Glad init
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to init GLAD" << std::endl;
  }

  // Viewport setup
  int window_width, window_height;
  glfwGetFramebufferSize(mWindow, &window_width, &window_height);
  glViewport(0, 0, window_width, window_height);
  glfwSetFramebufferSizeCallback(mWindow,
                                 [](GLFWwindow*, int width, int height) {
                                   GL(glViewport(0, 0, width, height));
                                 });

  // load and compile shaders
  m_shader_program_flat =
      LoadShader("../shaders/flat.vert", "../shaders/flat.frag");
}

void OpenGLRenderer::clear() {
  GL(glClearColor(0.4f, 0.2f, 0.4f, 1.0f));
  GL(glClear(GL_COLOR_BUFFER_BIT));
}

void OpenGLRenderer::finishRendering() {
  glfwPollEvents();
  glfwSwapBuffers(mWindow);
}

bool OpenGLRenderer::shouldClose() {
  return glfwWindowShouldClose(mWindow);
}

void OpenGLRenderer::drawCircle(Circle circle) {
  float scale = 1 / 10.0f;
  float vertices[] = {
      (circle.pos.x - circle.radius) * scale,
      (circle.pos.y - circle.radius) * scale,
      0.0f,
      (circle.pos.x + circle.radius) * scale,
      (circle.pos.y - circle.radius) * scale,
      0.0f,
      (circle.pos.x) * scale,
      (circle.pos.y + circle.radius) * scale,
      0.0f,
  };

  GLuint vao;
  GL(glGenVertexArrays(1, &vao));
  GL(glBindVertexArray(vao));

  GLuint vbo;
  GL(glGenBuffers(1, &vbo));
  GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));
  GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0));
  GL(glEnableVertexAttribArray(0));

  GL(glUseProgram(m_shader_program_flat));
  GL(glBindVertexArray(vao));
  GL(glDrawArrays(GL_TRIANGLES, 0, 3));
  GL(glDeleteVertexArrays(1, &vao));
  GL(glDeleteBuffers(1, &vbo));
}
