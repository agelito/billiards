#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "opengl.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"
#include "material.h"

typedef struct render_queue render_queue;

struct render_queue
{
    gl_functions* gl;

    uint32 queue_used;
    uint32 queue_capacity;
    uint8* queue_items;

    shader_uniform_group uniforms;
    shader_uniform_group uniforms_per_object;

    loaded_mesh text_buffer;
    uint32 text_buffer_capacity;
    uint32 text_buffer_count;
};

void
renderer_apply_uniforms(gl_functions* gl, shader_program* shader, shader_uniform_group* group);

render_queue
renderer_queue_create(gl_functions* gl, uint32 capacity, uint32 text_capacity);

void
renderer_queue_push_clear(render_queue* queue, uint32 clear_flags, float clear_color[4]);

void
renderer_queue_push_draw(render_queue* queue, loaded_mesh* mesh,
			 material* material, matrix4 transform);

void
renderer_queue_push_text(render_queue* queue, char* text, loaded_font* font,
			 real32 font_size, shader_program* shader, matrix4 transform);

void
renderer_queue_clear(render_queue* queue);

void
renderer_queue_process(render_queue* queue);

void
renderer_queue_set_projection(render_queue* queue, matrix4 projection);

void
renderer_queue_set_view(render_queue* queue, matrix4 view);

void
renderer_check_error();

#endif // RENDERER_H_INCLUDED
