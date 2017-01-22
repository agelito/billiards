#ifndef RENDERER_H_INCLUDED
#define RENDERER_H_INCLUDED

#include "opengl.h"
#include "shader.h"
#include "texture.h"
#include "mesh.h"

typedef struct render_queue render_queue;
typedef struct render_queue_item render_queue_item;

struct render_queue_item
{
    loaded_mesh* mesh;
    shader_program* shader;
    matrix4 transform;
};

struct render_queue
{
    gl_functions* gl;

    int count;
    int capacity;
    render_queue_item* items;

    shader_uniform_group uniforms;
    shader_uniform_group uniforms_per_object;
};

void
renderer_apply_uniforms(gl_functions* gl, shader_program* shader, shader_uniform_group* group);

void
renderer_draw_mesh(gl_functions* gl, loaded_mesh* mesh);

render_queue
renderer_queue_create(gl_functions* gl, int capacity);

void
renderer_queue_push(render_queue* queue, loaded_mesh* mesh,
		    shader_program* shader, matrix4 transform);

void
renderer_queue_clear(render_queue* queue);

void
renderer_queue_process(render_queue* queue, matrix4 projection, matrix4 view);

#endif // RENDERER_H_INCLUDED
