// opengl.c

#include "opengl.h"

#include "platform.h"

static void
print_gl_info()
{
    const GLubyte* vendor = glGetString(GL_VENDOR);
    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glsl_version = glGetString(GL_SHADING_LANGUAGE_VERSION);

    platform_log("%s (glsl %s)\n%s (%s)\n", version, glsl_version, renderer, vendor);
}

gl_functions load_gl_functions()
{
    print_gl_info();
    
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
    gl.glGetShaderInfoLog = GL_PROC_ADDR(glGetShaderInfoLog);
    gl.glCreateProgram = GL_PROC_ADDR(glCreateProgram);
    gl.glAttachShader = GL_PROC_ADDR(glAttachShader);
    gl.glLinkProgram = GL_PROC_ADDR(glLinkProgram);
    gl.glGetProgramiv = GL_PROC_ADDR(glGetProgramiv);
    gl.glGetProgramInfoLog = GL_PROC_ADDR(glGetProgramInfoLog);
    gl.glUseProgram = GL_PROC_ADDR(glUseProgram);
    
    gl.glGetUniformLocation = GL_PROC_ADDR(glGetUniformLocation);
    gl.glGetActiveUniform = GL_PROC_ADDR(glGetActiveUniform);
    gl.glGetAttribLocation = GL_PROC_ADDR(glGetAttribLocation);

    gl.glUniformMatrix2fv = GL_PROC_ADDR(glUniformMatrix2fv);
    gl.glUniformMatrix3fv = GL_PROC_ADDR(glUniformMatrix3fv);
    gl.glUniformMatrix4fv = GL_PROC_ADDR(glUniformMatrix4fv);

    gl.glUniform1fv = GL_PROC_ADDR(glUniform1fv);
    gl.glUniform2fv = GL_PROC_ADDR(glUniform2fv);
    gl.glUniform3fv = GL_PROC_ADDR(glUniform3fv);
    gl.glUniform4fv = GL_PROC_ADDR(glUniform4fv);

    gl.glUniform1iv = GL_PROC_ADDR(glUniform1iv);
    gl.glUniform2iv = GL_PROC_ADDR(glUniform2iv);
    gl.glUniform3iv = GL_PROC_ADDR(glUniform3iv);
    gl.glUniform4iv = GL_PROC_ADDR(glUniform4iv);
    
    gl.glGenerateMipmap = GL_PROC_ADDR(glGenerateMipmap);

    return gl;
}
