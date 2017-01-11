// renderer.c

#include "opengl.h"
#include "renderer.h"

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
	shader_uniform_data data = shader_uniform_get_data(group, uniform->location);
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
