// shader.c

#include "opengl.h"
#include "shader.h"

#include <stdio.h>

static void
print_shader_error(gl_functions* gl, GLuint shader)
{
    GLchar info_log[1024];
    gl->glGetShaderInfoLog(shader, 1024, NULL, info_log);
    printf("%s\n\n", info_log);
}

static void
print_program_error(gl_functions* gl, GLuint program)
{
    char info_log[1024];
    gl->glGetProgramInfoLog(program, 1024, NULL, (GLchar*)info_log);
    printf("%s\n\n", info_log);
}

shader_program
load_shader(gl_functions* gl, char* vertex_source, int vertex_source_length, char* fragment_source, int fragment_source_length)
{
    shader_program shader;

    shader.vertex = gl->glCreateShader(GL_VERTEX_SHADER);
    shader.fragment = gl->glCreateShader(GL_FRAGMENT_SHADER);

    gl->glShaderSource(shader.vertex, 1, (const GLchar**)&vertex_source, &vertex_source_length);
    gl->glShaderSource(shader.fragment, 1, (const GLchar**)&fragment_source, &fragment_source_length);

    int successful_compile = 0;
    
    gl->glCompileShader(shader.vertex);
    gl->glGetShaderiv(shader.vertex, GL_COMPILE_STATUS, &successful_compile);
    if(!successful_compile)
    {
        printf("failed to compile vertex shader:\n");
        print_shader_error(gl, shader.vertex);
    }

    gl->glCompileShader(shader.fragment);
    gl->glGetShaderiv(shader.fragment, GL_COMPILE_STATUS, &successful_compile);
    if(!successful_compile)
    {
        printf("failed to compile fragment shader:\n");
        print_shader_error(gl, shader.fragment);
    }

    shader.program = gl->glCreateProgram();
    gl->glAttachShader(shader.program, shader.vertex);
    gl->glAttachShader(shader.program, shader.fragment);
    gl->glLinkProgram(shader.program);

    int successful_link = 0;
    gl->glGetProgramiv(shader.program, GL_LINK_STATUS, &successful_link);
    if(!successful_link)
    {
        printf("failed to link shader program:\n");
        print_program_error(gl, shader.program);
    }

    return shader;
}
