#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "shader.h"
#include "mesh.h"

void renderer_apply_uniforms(gl_functions* gl, shader_program* shader, shader_uniform_group* group);

void renderer_draw_mesh(gl_functions* gl, loaded_mesh* mesh);

#endif // RENDERER_H_INCLUDED
