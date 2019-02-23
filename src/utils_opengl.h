#pragma once
#include "utils_opengl.h"
#include <glad/glad.h>


#ifdef _DEBUG
    #define GL_DEBUG
//Separate debug flag we can trigger for GL error checks
#endif

#ifdef GL_DEBUG
#define GL(stmt) do { \
            stmt; \
            CheckOpenGLError(#stmt, __FILE__, __LINE__); \
        } while (0)
#else
#define GL(stmt) stmt
#endif

void CheckOpenGLError(const char* stmt, const char* fname, int line);
GLuint LoadShader(const char *vertex_path, const char *fragment_path);
