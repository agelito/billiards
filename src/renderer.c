// renderer.c

#include "renderer.h"
#include "mesh.c"

#define glUniformScalar(type, u, c, d) gl->type(u->location, c, (void*)d)
#define glUniformMatrix(type, u, c, d) gl->type(u->location, c, GL_FALSE, (void*)d)

static void
renderer_upload_uniform(gl_functions* gl, shader_uniform* uniform, int count, unsigned char* data)
{
    switch(uniform->type)
    {
    case shader_data_float1:
	glUniformScalar(glUniform1fv, uniform, count, data);
	break;
    case shader_data_float2:
	glUniformScalar(glUniform2fv, uniform, count, data);
	break;
    case shader_data_float3:
	glUniformScalar(glUniform3fv, uniform, count, data);
	break;
    case shader_data_float4:
	glUniformScalar(glUniform4fv, uniform, count, data);
	break;
    case shader_data_integer1:
	glUniformScalar(glUniform1iv, uniform, count, data);
	break;
    case shader_data_integer2:
	glUniformScalar(glUniform2iv, uniform, count, data);
	break;
    case shader_data_integer3:
	glUniformScalar(glUniform3iv, uniform, count, data);
	break;
    case shader_data_integer4:
	glUniformScalar(glUniform4iv, uniform, count, data);
	break;
    case shader_data_matrix2:
	glUniformMatrix(glUniformMatrix2fv, uniform, count, data);
	break;
    case shader_data_matrix3:
	glUniformMatrix(glUniformMatrix3fv, uniform, count, data);
	break;
    case shader_data_matrix4:
	glUniformMatrix(glUniformMatrix4fv, uniform, count, data);
	break;
    default: break;
    }
}

void
renderer_apply_uniforms(gl_functions* gl, shader_program* shader, shader_uniform_group* group)
{
    shader_reflection* info = &shader->info;

    int uniform_slot;
    for(uniform_slot = 0; uniform_slot < info->uniform_count; ++uniform_slot)
    {
	shader_uniform* uniform = (info->uniforms + uniform_slot);
	shader_uniform_data data = shader_uniform_get_data(group, uniform->name_hash);
	if(!data.size) continue;

	int count = data.size / uniform->size_per_element;
	renderer_upload_uniform(gl, uniform, count, data.data);
    }
}

static void
renderer_bind_mesh_buffers(gl_functions* gl, loaded_mesh* mesh)
{
    gl->glBindVertexArray(mesh->vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
}

void
renderer_draw_mesh(gl_functions* gl, loaded_mesh* mesh)
{
    renderer_bind_mesh_buffers(gl, mesh);

    glDrawArrays(GL_TRIANGLES, 0, mesh->data.vertex_count);
}

render_queue
renderer_queue_create(gl_functions* gl, int capacity)
{
    render_queue queue = (render_queue){0};
    
    queue.gl = gl;
    queue.capacity = capacity;

    size_t queue_size = (sizeof(render_queue_item) * capacity);
    queue.items= (render_queue_item*)malloc(queue_size);

    queue.uniforms = shader_uniform_group_create(KB(1));
    queue.uniforms_per_object = shader_uniform_group_create(KB(1));

    return queue;
}

void
renderer_queue_push(render_queue* queue, loaded_mesh* mesh,
		    shader_program* shader, matrix4 transform)
{
    render_queue_item item;
    item.mesh = mesh;
    item.shader = shader;
    item.transform = transform;
    
    *(queue->items + queue->count++) = item;
}

void
renderer_queue_clear(render_queue* queue)
{
    queue->count = 0;
}

void
renderer_queue_process(render_queue* queue, matrix4 projection, matrix4 view)
{
    gl_functions* gl = queue->gl;
    
    loaded_mesh* bound_mesh = 0;
    shader_program* bound_shader = 0;

    shader_uniform_set_data(&queue->uniforms, hash_string("projection"),
			    projection.data, sizeof(matrix4));
    shader_uniform_set_data(&queue->uniforms, hash_string("view"),
			    view.data, sizeof(matrix4));

    vector3 tint_color = (vector3){{{1.0f, 1.0f, 1.0f}}};
    shader_uniform_set_data(&queue->uniforms, hash_string("tint_color"),
			    &tint_color, sizeof(vector3));
    
    int i;
    for_range(i, queue->count)
    {
	render_queue_item* item = (queue->items + i);

	shader_program* shader = item->shader;
	if(shader != bound_shader)
	{
	    gl->glUseProgram(shader->program);
	    renderer_apply_uniforms(gl, shader, &queue->uniforms);
	    
	    bound_shader = shader;
	}
	
	loaded_mesh* mesh = item->mesh;
	if(mesh != bound_mesh)
	{
	    renderer_bind_mesh_buffers(gl, mesh);
	    bound_mesh = mesh;
	}

	shader_uniform_set_data(&queue->uniforms_per_object, hash_string("world"),
				item->transform.data, sizeof(matrix4));

	renderer_apply_uniforms(gl, shader, &queue->uniforms_per_object);
	
	glDrawArrays(GL_TRIANGLES, 0, mesh->data.vertex_count);
    }
}

#undef glUniformScalar
#undef glUniformMatrix
