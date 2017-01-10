#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "shader.h"

void renderer_apply_uniforms(gl_functions* gl, shader_program* shader, shader_uniform_group* group);

#endif // RENDERER_H_INCLUDED
