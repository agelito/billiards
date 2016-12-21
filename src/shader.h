#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

typedef struct
{
    GLuint vertex;
    GLuint fragment;
    GLuint program;
} shader_program;

shader_program load_shader(gl_functions* gl, char* vertex_source, int vertex_source_length, char* fragment_source, int fragment_source_length);

#endif // SHADER_H_INCLUDED
