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
  mWindow = glfwCreateWindow(600, 600, "EvoView", nullptr, nullptr);
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
      LoadShader("shaders/flat.vert", "shaders/flat.frag");

  m_shader_program_circle =
      LoadShader("shaders/circle.vert", "shaders/circle.frag");
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
  /* A lot of this code should be split into init
   * and tear-down code. But for now this makes 
   * for easier reading even if it duplicates work*/

  float scale = 1 / 10.0f;
  float vertices[] = {
      //Vertex 0  Top right
      (circle.pos.x + circle.radius) * scale,
      (circle.pos.y + circle.radius) * scale,
      0.0f,
      1.0f, 1.0f,  // Texture coords

      //Vertex 1 Bottom right
      (circle.pos.x + circle.radius) * scale,
      (circle.pos.y - circle.radius) * scale,
      0.0f,
      1.0f, 0.0f,  // Texture coords

      //Vertex 2 Bottom left
      (circle.pos.x - circle.radius) * scale,
      (circle.pos.y - circle.radius) * scale,
      0.0f,
      0.0f, 0.0f,  // Texture coords

      //Vertex 3 Top left
      (circle.pos.x - circle.radius) * scale,
      (circle.pos.y + circle.radius) * scale,
      0.0f,
      0.0f, 1.0f,  // Texture coords
  };

  //Indices into vertices
  GLuint indices[] = {
    //Triangle 0
    0, 1, 3,
    //Triangle 1
    1, 2, 3
  };

  //Vertex array object to keep vertex attrib state
  GLuint vao;
  GL(glGenVertexArrays(1, &vao));

  //Vertex buffer object to keep the vertices
  GLuint vbo;
  GL(glGenBuffers(1, &vbo));

  //Element buffer object to keep the vertices
  GLuint ebo;
  GL(glGenBuffers(1, &ebo));

  //Make vao current
  GL(glBindVertexArray(vao));

  //Copy vertex data
  GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

  //Copy index array
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW));

  //Set vertex attribute pointers
  GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0));
  GL(glEnableVertexAttribArray(0));
  GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*) (3 * sizeof(float))));
  GL(glEnableVertexAttribArray(1));

  //Unbind buffers
  GL(glBindVertexArray(0)); //unbind vao
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); //Unbind ebo
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0)); //Unbind vbo

  //Set up state for draw
  GL(glUseProgram(m_shader_program_circle));
  GL(glBindVertexArray(vao)); //vao contains bindings to ebo and vbo
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  //DRAW
  GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

  //Cleanup
  GL(glDeleteVertexArrays(1, &vao));
  GL(glDeleteBuffers(1, &vbo));
  GL(glDeleteBuffers(1, &ebo));
}
