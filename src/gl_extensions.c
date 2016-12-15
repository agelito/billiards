// gl_extensions.c

#include "gl_extensions.h"

void load_gl_functions()
{
    glGenVertexArrays = GL_PROC_ADDR(glGenVertexArrays);
    glBindVertexArray = GL_PROC_ADDR(glBindVertexArray);
    glGenBuffers = GL_PROC_ADDR(glGenBuffers);
    glBindBuffer = GL_PROC_ADDR(glBindBuffer);
    glBufferData = GL_PROC_ADDR(glBufferData);
    glEnableVertexAttribArray = GL_PROC_ADDR(glEnableVertexAttribArray);
    glVertexAttribPointer = GL_PROC_ADDR(glVertexAttribPointer);

    glCreateShader = GL_PROC_ADDR(glCreateShader);
    glShaderSource = GL_PROC_ADDR(glShaderSource);
    glCompileShader = GL_PROC_ADDR(glCompileShader);
    glGetShaderiv = GL_PROC_ADDR(glGetShaderiv);
    glCreateProgram = GL_PROC_ADDR(glCreateProgram);
    glAttachShader = GL_PROC_ADDR(glAttachShader);
    glLinkProgram = GL_PROC_ADDR(glLinkProgram);
    glGetProgramiv = GL_PROC_ADDR(glGetProgramiv);
    glGetProgramInfoLog = GL_PROC_ADDR(glGetProgramInfoLog);
    glUseProgram = GL_PROC_ADDR(glUseProgram);
}
