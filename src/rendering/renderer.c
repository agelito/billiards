// renderer.c

#include "renderer.h"

#include "mesh.c"
#include "shader.c"
#include "texture.c"
#include "material.c"

#define glUniformScalar(type, u, c, d) gl->type(u->location, c, (void*)d)
#define glUniformMatrix(type, u, c, d) gl->type(u->location, c, GL_FALSE, (void*)d)

typedef enum render_queue_type render_queue_type;

typedef struct render_queue_item render_queue_item;
typedef struct render_queue_clear render_queue_clear;
typedef struct render_queue_draw render_queue_draw;

enum render_queue_type
{
    render_queue_type_clear,
    render_queue_type_draw
};

struct render_queue_item
{
    render_queue_type type;
    uint16 command_size;
};

struct render_queue_clear
{
    float clear_color[4];
    uint32 clear_flags;
};

struct render_queue_draw
{
    loaded_mesh* mesh;
    material* material;
    matrix4 transform;

    uint32 draw_element_offset;
    uint32 draw_element_count;
};

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
    case shader_data_sampler2d:
	glUniformScalar(glUniform1iv, uniform, count, data);
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

static input_layout*
mesh_layout_for_shader(gl_functions* gl, loaded_mesh* mesh, shader_program* shader)
{
    input_layout* layout = 0;

    int shader_attributes = 0;
    int position_location = gl->glGetAttribLocation(shader->program, "in_vertex");
    int color_location = gl->glGetAttribLocation(shader->program, "in_color");
    int texcoord_location = gl->glGetAttribLocation(shader->program, "in_uv");
    int normal_location = gl->glGetAttribLocation(shader->program, "in_normal");
    int tangent_location = gl->glGetAttribLocation(shader->program, "in_tangent");
    int binormal_location = gl->glGetAttribLocation(shader->program, "in_binormal");

    if(position_location != -1) VA_INCLUDE(shader_attributes, VA_POSITIONS_BIT);
    if(normal_location != -1)   VA_INCLUDE(shader_attributes, VA_NORMALS_BIT);
    if(texcoord_location != -1) VA_INCLUDE(shader_attributes, VA_TEXCOORDS_BIT);
    if(color_location != -1)    VA_INCLUDE(shader_attributes, VA_COLORS_BIT);
    if(tangent_location != -1)  VA_INCLUDE(shader_attributes, VA_TANGENTS_BIT);
    if(binormal_location != -1) VA_INCLUDE(shader_attributes, VA_BINORMALS_BIT);

    int attribute_mask = (shader_attributes & mesh->attribute_mask);
    
    int i;
    for_range(i, mesh->layout_count)
    {
	input_layout* check = (mesh->layouts + i);
	if(check->attribute_mask == attribute_mask)
	{
	    layout = check;
	    break;
	}
    }
    
    if(!layout)
    {
	assert(mesh->layout_count < MESH_MAX_INPUT_LAYOUTS);
	layout = (mesh->layouts + mesh->layout_count++);
	layout->attribute_mask = attribute_mask;

	gl->glGenVertexArrays(1, &layout->vertex_array);
	gl->glBindVertexArray(layout->vertex_array);

	if(mesh->index_buffer)
	{
	    gl->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh->index_buffer);
	}

	if(VA_ISSET(attribute_mask, VA_POSITIONS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_positions));
	    gl->glEnableVertexAttribArray(position_location);
	    gl->glVertexAttribPointer(position_location, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), 0);
	}

	if(VA_ISSET(attribute_mask, VA_NORMALS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_normals));
	    gl->glEnableVertexAttribArray(normal_location);
	    gl->glVertexAttribPointer(normal_location, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), 0);
	}

	if(VA_ISSET(attribute_mask, VA_TEXCOORDS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_texcoords));
	    gl->glEnableVertexAttribArray(texcoord_location);
	    gl->glVertexAttribPointer(texcoord_location, 2, GL_FLOAT, GL_FALSE, sizeof(vector2), 0);
	}

	if(VA_ISSET(attribute_mask, VA_COLORS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_colors));
	    gl->glEnableVertexAttribArray(color_location);
	    gl->glVertexAttribPointer(color_location, 3, GL_FLOAT, GL_FALSE, sizeof(color), 0);
	}

	if(VA_ISSET(attribute_mask, VA_TANGENTS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_tangents));
	    gl->glEnableVertexAttribArray(tangent_location);
	    gl->glVertexAttribPointer(tangent_location, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), 0);
	}

	if(VA_ISSET(attribute_mask, VA_BINORMALS_BIT))
	{
	    gl->glBindBuffer(GL_ARRAY_BUFFER, *(mesh->vertex_buffer + vertex_attributes_binormals));
	    gl->glEnableVertexAttribArray(binormal_location);
	    gl->glVertexAttribPointer(binormal_location, 3, GL_FLOAT, GL_FALSE, sizeof(vector3), 0);
	}
    }

    return layout;
}

static void
renderer_bind_mesh_buffers(gl_functions* gl, loaded_mesh* mesh, shader_program* shader)
{
    input_layout* layout = mesh_layout_for_shader(gl, mesh, shader);
    if(layout)
    {
	gl->glBindVertexArray(layout->vertex_array);
    }
}

render_queue
renderer_queue_create(gl_functions* gl, uint32 capacity, uint32 text_capacity)
{
    render_queue queue = (render_queue){0};
    
    queue.gl = gl;
    queue.queue_used = 0;
    queue.queue_capacity = capacity;

    queue.queue_items = (uint8*)malloc(capacity);

    queue.uniforms = shader_uniform_group_create(KB(1));
    queue.uniforms_per_object = shader_uniform_group_create(KB(1));

    mesh_data text_mesh_data = (mesh_data){0};
    text_mesh_data.vertex_count = (text_capacity * 6);

    size_t position_data_size = (text_mesh_data.vertex_count * sizeof(vector3));
    text_mesh_data.vertices.positions = (vector3*)malloc(position_data_size);

    size_t texcoord_data_size = (text_mesh_data.vertex_count * sizeof(vector2));
    text_mesh_data.vertices.texcoords = (vector2*)malloc(texcoord_data_size);

    memset(text_mesh_data.vertices.positions, 0, position_data_size);
    memset(text_mesh_data.vertices.texcoords, 0, texcoord_data_size);

    queue.text_buffer             = load_mesh(gl, text_mesh_data, 1);
    queue.text_buffer_capacity    = text_mesh_data.vertex_count;
    queue.text_buffer_count       = 0;

    renderer_check_error();

    return queue;
}

static inline void
renderer_queue_push_item(render_queue* queue, render_queue_type type,
			 void* command_data, uint16 command_size)
{
    uint32 push_size = sizeof(render_queue_item) + command_size;
    if((queue->queue_used + push_size) > queue->queue_capacity)
    {
	platform_log("render queue capacity full (%d/%d)\n",
		     queue->queue_used + push_size,
		     queue->queue_capacity);
	return;
    }
    
    render_queue_item* item = (render_queue_item*)(queue->queue_items + queue->queue_used);
    
    item->type         = type;
    item->command_size = command_size;

    uint32 push_data_offset = (queue->queue_used + sizeof(render_queue_item));
    
    platform_copy_memory((queue->queue_items + push_data_offset), command_data, command_size);

    queue->queue_used += push_size;
}

void
renderer_queue_push_clear(render_queue* queue, uint32 clear_flags, float clear_color[4])
{
    render_queue_clear clear;
    
    clear.clear_flags = clear_flags;
    
    clear.clear_color[0] = clear_color[0];
    clear.clear_color[1] = clear_color[1];
    clear.clear_color[2] = clear_color[2];
    clear.clear_color[3] = clear_color[3];

    renderer_queue_push_item(queue, render_queue_type_clear, &clear, sizeof(render_queue_clear));
}

void
renderer_queue_push_draw(render_queue* queue, loaded_mesh* mesh, material* material, matrix4 transform)
{
    render_queue_draw draw;
    draw.mesh = mesh;
    draw.material = material;
    draw.transform = transform;

    if(mesh->data.index_count)
    {
	draw.draw_element_offset = 0;
	draw.draw_element_count = mesh->data.index_count;
    }
    else
    {
	draw.draw_element_offset = 0;
	draw.draw_element_count = mesh->data.vertex_count;
    }

    renderer_queue_push_item(queue, render_queue_type_draw, &draw, sizeof(render_queue_draw));
}

void
renderer_queue_push_text(render_queue* queue, char* text, loaded_font* font,
			 real32 font_size, shader_program* shader, matrix4 transform)
{
    render_queue_draw draw;
    draw.mesh      = &queue->text_buffer;
    draw.transform = transform;
    
    int32 cursor_x = 0;
    int32 cursor_y = 0;

    vector3* vertex_positions =
	(queue->text_buffer.data.vertices.positions + queue->text_buffer_count);
    vector2* vertex_texcoords =
	(queue->text_buffer.data.vertices.texcoords + queue->text_buffer_count);

    uint32 submit_vertex_count = 0;

    real32 one_over_page_width = 0.0f;
    real32 one_over_page_height = 0.0f;
    real32 one_over_size = (1.0f / font->data.size);

    material* configured_material = 0;
    
    char character, previous = 0;
    while((character = *text++) != 0)
    {
	// TODO: Make sure the text vertex buffer isn't overflowed.
	
	font_kerning* kern = font_get_kerning(&font->data, (uint32)previous, (uint32)character);
	font_character* fc = font_get_character(&font->data, (uint32)character);
	if(fc != 0)
	{
	    material* page_material = (font->materials + fc->page);
	    if(page_material != configured_material)
	    {
		loaded_texture* page_texture = (font->textures + fc->page);
	    
		if(submit_vertex_count)
		{
		    draw.draw_element_offset = queue->text_buffer_count;
		    draw.draw_element_count  = submit_vertex_count;
		    
		    renderer_queue_push_item(queue, render_queue_type_draw, &draw,
					     sizeof(render_queue_draw));

		    queue->text_buffer_count += submit_vertex_count;

		    submit_vertex_count = 0;
		}

		one_over_page_width = (1.0f / page_texture->data.width);
		one_over_page_height = (1.0f / page_texture->data.height);
		
		configured_material = page_material;
		configured_material->shader = shader;
		
		draw.material = page_material;
	    }

	    int32 base_x = cursor_x;
	    int32 base_y = cursor_y + font->data.baseline;

	    int32 x_min = base_x + fc->offset_x;
	    int32 x_max = x_min + fc->source_w;
	    int32 y_max = base_y - fc->offset_y;
	    int32 y_min = y_max - fc->source_h;

	    if(kern != 0)
	    {
		x_min += kern->amount;
		x_max += kern->amount;
	    }

	    real32 left   = (real32)x_min * one_over_size * font_size;
	    real32 right  = (real32)x_max * one_over_size * font_size;
	    
	    real32 bottom = (real32)y_min * one_over_size * font_size;
	    real32 top    = (real32)y_max * one_over_size * font_size;

	    *(vertex_positions + 0) = vector3_create(left, bottom, 0.0f);
	    *(vertex_positions + 1) = vector3_create(right, bottom, 0.0f);
	    *(vertex_positions + 2) = vector3_create(left, top, 0.0f);
	    *(vertex_positions + 3) = vector3_create(right, bottom, 0.0f);
	    *(vertex_positions + 4) = vector3_create(right, top, 0.0f);
	    *(vertex_positions + 5) = vector3_create(left, top, 0.0f);
	    vertex_positions += 6;

	    real32 uv_x0 = one_over_page_width * fc->source_x;
	    real32 uv_x1 = one_over_page_width * (fc->source_x + fc->source_w);

	    real32 uv_y1 = (one_over_page_height * fc->source_y);
	    real32 uv_y0 = (one_over_page_height * (fc->source_y + fc->source_h));

	    *(vertex_texcoords + 0) = vector2_create(uv_x0, uv_y0);
	    *(vertex_texcoords + 1) = vector2_create(uv_x1, uv_y0);
	    *(vertex_texcoords + 2) = vector2_create(uv_x0, uv_y1);
	    *(vertex_texcoords + 3) = vector2_create(uv_x1, uv_y0);
	    *(vertex_texcoords + 4) = vector2_create(uv_x1, uv_y1);
	    *(vertex_texcoords + 5) = vector2_create(uv_x0, uv_y1);
	    vertex_texcoords += 6;

	    submit_vertex_count += 6;

	    cursor_x += fc->advance;
	}
	else
	{
	    if(character == '\n')
	    {
		cursor_x = 0;
		cursor_y -= font->data.line_height;
	    }
	}

	previous = character;
    }

    if(submit_vertex_count)
    {
	draw.draw_element_offset = queue->text_buffer_count;
	draw.draw_element_count  = submit_vertex_count;	

	renderer_queue_push_item(queue, render_queue_type_draw, &draw,
				 sizeof(render_queue_draw));

	queue->text_buffer_count += submit_vertex_count;
	submit_vertex_count = 0;
    }
}

void
renderer_queue_clear(render_queue* queue)
{
    queue->queue_used = 0;
    queue->text_buffer_count = 0;
}

static void
configure_for_transparency(bool32 transparency_enabled)
{
    if(transparency_enabled)
    {
	glDisable(GL_DEPTH_TEST);
		
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }
    else
    {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
		
	glDisable(GL_BLEND);
    }
}

void
renderer_queue_process(render_queue* queue)
{
    gl_functions* gl = queue->gl;
    
    loaded_mesh* bound_mesh = 0;
    material* bound_material = 0;

    if(queue->text_buffer_count)
    {
	update_mesh(queue->gl, &queue->text_buffer, 0, queue->text_buffer_count);
    }

    uint32 queue_processed = 0;
    while(queue_processed < queue->queue_used)
    {
	uint8* queue_head = (queue->queue_items + queue_processed);
	render_queue_item* item = (render_queue_item*)queue_head;
	switch(item->type)
	{
	case render_queue_type_clear: {
	    render_queue_clear* clear =
		(render_queue_clear*)(queue_head + sizeof(render_queue_item));

	    glClearColor(clear->clear_color[0], clear->clear_color[1],
			 clear->clear_color[2], clear->clear_color[3]);
	    glClear(clear->clear_flags);

	} break;
	case render_queue_type_draw: {
	    render_queue_draw* draw =
		(render_queue_draw*)(queue_head + sizeof(render_queue_item));

	    int rebind_mesh = 0;
	    
	    material* material = draw->material;
	    shader_program* shader = material->shader;
	    if(material != bound_material)
	    {
		gl->glUseProgram(shader->program);
	    
		configure_for_transparency(shader->transparent);
	 
		renderer_apply_uniforms(gl, shader, &queue->uniforms);

		material_apply(material, &queue->uniforms_per_object);
	    
		bound_material = material;
		rebind_mesh = 1;
	    }
	
	    loaded_mesh* mesh = draw->mesh;
	    if(mesh != bound_mesh || rebind_mesh)
	    {
		renderer_bind_mesh_buffers(gl, mesh, shader);
		bound_mesh = mesh;
	    }

	    shader_uniform_set_data(&queue->uniforms_per_object, hash_string("world"),
				    draw->transform.data, sizeof(matrix4));

	    renderer_apply_uniforms(gl, shader, &queue->uniforms_per_object);

	    if(!mesh->index_buffer)
	    {
		glDrawArrays(GL_TRIANGLES, draw->draw_element_offset, draw->draw_element_count);
	    }
	    else
	    {
		glDrawElements(GL_TRIANGLES, draw->draw_element_count,
			       GL_UNSIGNED_INT, (void*)0);
	    }
	} break;
	}

	queue_processed += (sizeof(render_queue_item) + item->command_size);
    }

    renderer_check_error();
}

void
renderer_queue_set_projection(render_queue* queue, matrix4 projection)
{
    shader_uniform_set_data(&queue->uniforms, hash_string("projection"),
			    projection.data, sizeof(matrix4));
}

void
renderer_queue_set_view(render_queue* queue, matrix4 view)
{
    shader_uniform_set_data(&queue->uniforms, hash_string("view"),
			    view.data, sizeof(matrix4));
}

void
renderer_check_error()
{
    GLenum error;
    while((error = glGetError()) != GL_NO_ERROR)
    {
	char* error_name = "";
	switch(error)
	{
	case GL_INVALID_ENUM:
	    error_name = "GL_INVALID_ENUM";
	    break;
	case GL_INVALID_VALUE:
	    error_name = "GL_INVALID_VALUE";
	    break;
	case GL_INVALID_OPERATION:
	    error_name = "GL_INVALID_OPERATION";
	    break;
	case GL_INVALID_FRAMEBUFFER_OPERATION:
	    error_name = "GL_INVALID_FRAMEBUFFER_OPERATION";
	    break;
	case GL_OUT_OF_MEMORY:
	    error_name = "GL_OUT_OF_MEMORY";
	    break;
	case GL_STACK_UNDERFLOW:
	    error_name = "GL_STACK_UNDERFLOW";
	    break;
	case GL_STACK_OVERFLOW:
	    error_name = "GL_STACK_OVERFLOW";
	    break;
	}

	platform_log("OpenGL Error: %s (%d)\n", error_name, error);
    }
}

#undef glUniformScalar
#undef glUniformMatrix
