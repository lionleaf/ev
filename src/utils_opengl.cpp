#include "utils_opengl.h"
#include <iostream>
#include <string>
#include <vector>
#include "utils.h"
namespace ev {
void CheckOpenGLError(const char* stmt, const char* fname, int line) {
  GLenum err = glGetError();
  if (err != GL_NO_ERROR) {
    printf("OpenGL error %08x, at %s:%i - for %s\n", err, fname, line, stmt);
    // abort();
  }
}

GLuint LoadProgram(const char* vertex_path, const char* fragment_path) {
  GLuint vertShader = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

  // Read shaders
  std::string vertShaderStr = readFile(vertex_path);
  std::string fragShaderStr = readFile(fragment_path);
  const char* vertShaderSrc = vertShaderStr.c_str();
  const char* fragShaderSrc = fragShaderStr.c_str();

  GLint result = GL_FALSE;

  // Compile vertex shader
  std::cout << "Compiling vertex shader." << std::endl;
  glShaderSource(vertShader, 1, &vertShaderSrc, nullptr);
  glCompileShader(vertShader);

#ifdef GL_DEBUG
  int logLength;

  // Check vertex shader
  glGetShaderiv(vertShader, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    glGetShaderiv(vertShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<GLchar> vertShaderError((logLength > 1) ? logLength : 1);
    GL(glGetShaderInfoLog(vertShader, logLength, nullptr, &vertShaderError[0]));
    std::cout << &vertShaderError[0] << std::endl;
  }
#endif

  // Compile fragment shader
  glShaderSource(fragShader, 1, &fragShaderSrc, nullptr);
  glCompileShader(fragShader);

#ifdef GL_DEBUG
  // Check fragment shader
  glGetShaderiv(fragShader, GL_COMPILE_STATUS, &result);

  if (result == GL_FALSE) {
    glGetShaderiv(fragShader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<GLchar> fragShaderError((logLength > 1) ? logLength : 1);
    glGetShaderInfoLog(fragShader, logLength, nullptr, &fragShaderError[0]);
    std::cout << &fragShaderError[0] << std::endl;
  }
#endif

  GLuint program = glCreateProgram();
  glAttachShader(program, vertShader);
  glAttachShader(program, fragShader);
  glLinkProgram(program);

#ifdef GL_DEBUG
  // Check linking status
  glGetProgramiv(program, GL_LINK_STATUS, &result);

  if (result == GL_FALSE) {
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> programError((logLength > 1) ? logLength : 1);
    glGetProgramInfoLog(program, logLength, nullptr, &programError[0]);
    std::cout << &programError[0] << std::endl;
  }
#endif

  GL(glDeleteShader(vertShader));
  GL(glDeleteShader(fragShader));

  return program;
}
}  // namespace ev
