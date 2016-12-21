// gl_extensions.c

#include "gl_extensions.h"

gl_functions load_gl_functions()
{
    gl_functions gl = (gl_functions){0};
    gl.glGenVertexArrays = GL_PROC_ADDR(glGenVertexArrays);
    gl.glBindVertexArray = GL_PROC_ADDR(glBindVertexArray);
    gl.glGenBuffers = GL_PROC_ADDR(glGenBuffers);
    gl.glBindBuffer = GL_PROC_ADDR(glBindBuffer);
    gl.glBufferData = GL_PROC_ADDR(glBufferData);
    gl.glEnableVertexAttribArray = GL_PROC_ADDR(glEnableVertexAttribArray);
    gl.glVertexAttribPointer = GL_PROC_ADDR(glVertexAttribPointer);

    gl.glCreateShader = GL_PROC_ADDR(glCreateShader);
    gl.glShaderSource = GL_PROC_ADDR(glShaderSource);
    gl.glCompileShader = GL_PROC_ADDR(glCompileShader);
    gl.glGetShaderiv = GL_PROC_ADDR(glGetShaderiv);
    gl.glCreateProgram = GL_PROC_ADDR(glCreateProgram);
    gl.glAttachShader = GL_PROC_ADDR(glAttachShader);
    gl.glLinkProgram = GL_PROC_ADDR(glLinkProgram);
    gl.glGetProgramiv = GL_PROC_ADDR(glGetProgramiv);
    gl.glGetProgramInfoLog = GL_PROC_ADDR(glGetProgramInfoLog);
    gl.glUseProgram = GL_PROC_ADDR(glUseProgram);
    
    gl.glGetUniformLocation = GL_PROC_ADDR(glGetUniformLocation);
    gl.glUniformMatrix4fv = GL_PROC_ADDR(glUniformMatrix4fv);

    return gl;
}
