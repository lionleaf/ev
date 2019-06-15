#include "renderer_opengl.h"
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <string>
#include <vector>
#include "ev_ui.h"
#include "utils.h"
#include "utils_opengl.h"

void GLFWErrorCallback(int i, const char* err_str) {
  std::cout << "GLFW Error: " << err_str << std::endl;
}

OpenGLRenderer::OpenGLRenderer() {
  // GLFW init
  glfwSetErrorCallback(GLFWErrorCallback);
  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);  // Needed for Mac

  // Window creation
  m_window = glfwCreateWindow(m_window_width, m_window_height, "EvoView",
                              nullptr, nullptr);
  if (m_window == nullptr) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
  }
  glfwMakeContextCurrent(m_window);

  glfwSetWindowUserPointer(m_window, this);

  auto scroll_callback_redirect = [](GLFWwindow* w, double offset_x,
                                     double offset_y) {
    static_cast<OpenGLRenderer*>(glfwGetWindowUserPointer(w))
        ->scroll_callback(w, offset_y);
  };

  glfwSetScrollCallback(m_window, scroll_callback_redirect);

  // Glad init
  if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress))) {
    std::cout << "Failed to init GLAD" << std::endl;
  }

  // Set up the UI
  ev_ui::init(m_window);

  // Viewport setup
  glfwGetFramebufferSize(m_window, &m_window_width, &m_window_height);
  glViewport(0, 0, m_window_width, m_window_height);
  glfwSetFramebufferSizeCallback(m_window,
                                 [](GLFWwindow*, int width, int height) {
                                   GL(glViewport(0, 0, width, height));
                                 });

  // load and compile shaders
  m_flat_shader_program = LoadProgram("shaders/flat.vert", "shaders/flat.frag");
  m_flat_transform_loc =
      glGetUniformLocation(m_flat_shader_program, "transform");

  m_circle_shader_program =
      LoadProgram("shaders/circle.vert", "shaders/circle.frag");
  m_circle_transform_loc =
      glGetUniformLocation(m_circle_shader_program, "transform");

  m_camera.pos = glm::vec3{0, 0, 50};  // Look down -Z axis.
  m_camera.look_at = glm::vec3{0, 0, 0};
  m_camera.up = glm::vec3{0, 1, 0};  // Positive Y is up

  m_view_matrix = glm::lookAt(m_camera.pos, m_camera.look_at, m_camera.up);
  m_projection_matrix = glm::perspective(
      glm::radians(45.0f), float(m_window_width) / float(m_window_height), 0.1f,
      1000.0f);

  m_quad_vao = setup_quad();

  GL(glEnable(GL_BLEND));
  GL(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
}

OpenGLRenderer::~OpenGLRenderer() {
  ev_ui::destroy();
  glfwDestroyWindow(m_window);
  glfwTerminate();
}

void OpenGLRenderer::start_frame() {
  glfwPollEvents();
  poll_camera_input();

  m_view_matrix = glm::lookAt(m_camera.pos, m_camera.look_at, m_camera.up);

  ev_ui::start_frame();

  GL(glClearColor(0.4f, 0.2f, 0.4f, 1.0f));
  GL(glClear(GL_COLOR_BUFFER_BIT));
}

void OpenGLRenderer::scroll_callback(GLFWwindow* window, double offset) {
  m_camera.pos.z -= offset;
}

void OpenGLRenderer::poll_camera_input() {
  if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
    m_camera.pos.y += 0.3;
    m_camera.look_at.y += 0.3;
  }
  if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
    m_camera.pos.y -= 0.3;
    m_camera.look_at.y -= 0.3;
  }
  if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
    m_camera.pos.x += 0.3;
    m_camera.look_at.x += 0.3;
  }
  if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
    m_camera.pos.x -= 0.3;
    m_camera.look_at.x -= 0.3;
  }
}

void OpenGLRenderer::end_frame() {
  ev_ui::draw();

  // Swap buffers
  glfwMakeContextCurrent(m_window);
  glfwSwapBuffers(m_window);
}

bool OpenGLRenderer::should_close() {
  return glfwWindowShouldClose(m_window);
}

GLuint OpenGLRenderer::setup_quad() {
  float vertices[] = {
      // Vertex 0  Top right
      1.0f, 1.0f, 0.0f, 1.0f,
      1.0f,  // Texture coords

      // Vertex 1 Bottom right
      1.0f, -1.0f, 0.0f, 1.0f,
      0.0f,  // Texture coords

      // Vertex 2 Bottom left
      -1.0f, -1.0f, 0.0f, 0.0f,
      0.0f,  // Texture coords

      // Vertex 3 Top left
      -1.0f, 1.0f, 0.0f, 0.0f,
      1.0f,  // Texture coords
  };

  // Indices into vertices
  GLuint indices[] = {// Triangle 0
                      0, 1, 3,
                      // Triangle 1
                      1, 2, 3};

  // Vertex array object to keep vertex attrib state
  GLuint vao{};
  GL(glGenVertexArrays(1, &vao));

  // Vertex buffer object to keep the vertices
  GLuint vbo;
  GL(glGenBuffers(1, &vbo));

  // Element buffer object to keep the vertices
  GLuint ebo;
  GL(glGenBuffers(1, &ebo));

  // Make vao current
  GL(glBindVertexArray(vao));

  // Copy vertex data
  GL(glBindBuffer(GL_ARRAY_BUFFER, vbo));
  GL(glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW));

  // Copy index array
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo));
  GL(glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
                  GL_STATIC_DRAW));

  // Set vertex attribute pointers
  GL(glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0));
  GL(glEnableVertexAttribArray(0));
  GL(glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
                           (void*)(3 * sizeof(float))));
  GL(glEnableVertexAttribArray(1));

  // Unbind buffers
  GL(glBindVertexArray(0));                      // unbind vao
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));  // Unbind ebo
  GL(glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0));  // Unbind vbo

  return vao;
}

void OpenGLRenderer::draw_circle(Circle circle, Vec2f offset) {
  /* A lot of this code should be split into init
   * and tear-down code. But for now this makes
   * for easier reading even if it duplicates work*/
  Vec2f pos = offset + circle.pos;

  auto model_to_world = glm::mat4{1.0};
  model_to_world =
      glm::translate(model_to_world, glm::vec3{pos.x, pos.y, 0.0f});
  model_to_world =
      glm::scale(model_to_world, glm::vec3{circle.radius, circle.radius, 1.0f});
  // rotate

  auto transform = m_projection_matrix * m_view_matrix * model_to_world;

  // Set up state for draw
  GL(glUseProgram(m_circle_shader_program));
  GL(glBindVertexArray(m_quad_vao));  // vao contains bindings to ebo and vbo

  // Uniforms
  GL(glUniformMatrix4fv(m_circle_transform_loc, 1, GL_FALSE,
                        glm::value_ptr(transform)));

  // DRAW
  GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

  // Unbind
  GL(glBindVertexArray(0));
  GL(glUseProgram(0));
}

void OpenGLRenderer::draw_rect(AABB rect, Vec2f offset) {
  /* A lot of this code should be split into init
   * and tear-down code. But for now this makes
   * for easier reading even if it duplicates work*/

  // offset = {0.0, 0.0};
  Vec2f extent = (rect.max - rect.min) / 2.0f;
  Vec2f pos = offset + rect.min + extent;

  auto model_to_world = glm::mat4{1.0};
  model_to_world =
      glm::translate(model_to_world, glm::vec3{pos.x, pos.y, 0.0f});
  model_to_world =
      glm::scale(model_to_world, glm::vec3{extent.x, extent.y, 1.0f});
  // rotate

  auto transform = m_projection_matrix * m_view_matrix * model_to_world;

  // Set up state for draw
  GL(glUseProgram(m_flat_shader_program));
  GL(glBindVertexArray(m_quad_vao));  // vao contains bindings to ebo and vbo

  // Uniforms
  GL(glUniformMatrix4fv(m_flat_transform_loc, 1, GL_FALSE,
                        glm::value_ptr(transform)));

  // DRAW
  GL(glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0));

  // Unbind
  GL(glBindVertexArray(0));  // vao contains bindings to ebo and vbo
  GL(glUseProgram(0));
}

void OpenGLRenderer::draw_creature(Creature& creature) {
  draw_body(creature.body());
}

void OpenGLRenderer::draw_body(const Body& body) {
  for (Circle circle : body.circles) {
    draw_circle(circle, body.pos);
  }
  for (AABB rect : body.rects) {
    draw_rect(rect, body.pos);
  }
}
