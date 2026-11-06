// ====================================================================
// FILE:     src/core/glad.cpp
// OWNER:    Member 1
// PURPOSE:  Provides OpenGL function loader implementation (GLAD 2)
// ====================================================================

#define GLAD_GL_IMPLEMENTATION
#if __has_include(<glad/glad.h>)
#include <glad/glad.h>
#elif __has_include(<glad/gl.h>)
#include <glad/gl.h>
#endif

