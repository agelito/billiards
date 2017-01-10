// shader.c

#include "platform.h"
#include "opengl.h"
#include "shader.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define UNIFORM_BLOCK_CAPACITY 32

typedef struct uniform_data_location uniform_data_location;
struct uniform_data_location
{
    int location;
    size_t offset;
    size_t size;
};

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

    shader.info = shader_reflect(gl, &shader);

    return shader;
}

static shader_data_type
shader_type_from_gl(GLenum type)
{
    shader_data_type result = shader_data_unknown;
    switch(type)
    {
    case GL_FLOAT:      result = shader_data_float1; break;
    case GL_FLOAT_VEC2: result = shader_data_float2; break;
    case GL_FLOAT_VEC3:	result = shader_data_float3; break;
    case GL_FLOAT_VEC4:	result = shader_data_float4; break;
    case GL_INT:	result = shader_data_integer1; break;
    case GL_INT_VEC2:	result = shader_data_integer2; break;
    case GL_INT_VEC3:	result = shader_data_integer3; break;
    case GL_INT_VEC4:	result = shader_data_integer4; break;
    case GL_FLOAT_MAT2:	result = shader_data_matrix2; break;
    case GL_FLOAT_MAT3:	result = shader_data_matrix3; break;
    case GL_FLOAT_MAT4:	result = shader_data_matrix4; break;
    }
    return result;
}

size_t
shader_data_type_size(shader_data_type type, int count)
{
    size_t base_size = 0;
    switch(type)
    {
    case shader_data_float1: base_size = sizeof(float) * 1; break;
    case shader_data_float2: base_size = sizeof(float) * 2; break;
    case shader_data_float3: base_size = sizeof(float) * 3; break;
    case shader_data_float4: base_size = sizeof(float) * 4; break;
    case shader_data_integer1: base_size = sizeof(int) * 1; break;
    case shader_data_integer2: base_size = sizeof(int) * 2; break;
    case shader_data_integer3: base_size = sizeof(int) * 3; break;
    case shader_data_integer4: base_size = sizeof(int) * 4; break;
    case shader_data_matrix2: base_size = sizeof(float) * 4; break;
    case shader_data_matrix3: base_size = sizeof(float) * 9; break;
    case shader_data_matrix4: base_size = sizeof(float) * 16; break;
    default: base_size = 0; break;
    }

    return (base_size * count);
}

shader_reflection
shader_reflect(gl_functions* gl, shader_program* shader)
{
    int uniform_count;
    gl->glGetProgramiv(shader->program, GL_ACTIVE_UNIFORMS, &uniform_count);
    
    int max_name_length;
    gl->glGetProgramiv(shader->program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &max_name_length);

    char* uniform_name = (char*)malloc(max_name_length + 1);
    
    shader_reflection group = (shader_reflection){0};
    group.uniform_count = uniform_count;
    group.uniforms = (shader_uniform*)malloc(sizeof(shader_uniform) * uniform_count);

    int uniform_index;
    for(uniform_index = 0; uniform_index < uniform_count; ++uniform_index)
    {
        int name_length;
        int uniform_count;
        GLenum type;

        gl->glGetActiveUniform(shader->program, uniform_index, max_name_length,
                               &name_length, &uniform_count, &type, uniform_name);
        *(uniform_name + name_length) = 0;

        shader_uniform uniform_data = (shader_uniform){0};
        uniform_data.name = (char*)malloc(name_length + 1);
        platform_copy_memory(uniform_data.name, uniform_name, name_length + 1);
	
        uniform_data.location = uniform_index;

        shader_data_type data_type = shader_type_from_gl(type);
        if(data_type == shader_data_unknown)
        {
            fprintf(stderr, "unrecognized uniform %s at %d, type: %d\n",
                    uniform_name, uniform_index, type);
            continue;
        }

        uniform_data.type = data_type;
	uniform_data.size_per_element = 
	    shader_data_type_size(data_type, 1);

        *(group.uniforms + uniform_index) = uniform_data;
    }

    free(uniform_name);
    uniform_name = 0;

    return group;
}

shader_uniform*
shader_get_uniform(shader_reflection* reflection, char* uniform_name)
{
    shader_uniform* result = 0;
    
    int i;
    for(i = 0; i < reflection->uniform_count; i++)
    {
        shader_uniform* search = (reflection->uniforms + i);
        if(strcmp(search->name, uniform_name) == 0)
        {
            result = search;
            break;
        }
    }
    
    return result;
}

static void
allocate_uniform_list(uniform_data_location_list* uniform_list)
{
    uniform_list->locations =
	(uniform_data_location*)malloc(sizeof(uniform_data_location) * UNIFORM_BLOCK_CAPACITY);
			       
    uniform_list->count = 0;
    uniform_list->next = 0;
}

shader_uniform_group
shader_uniform_group_create(size_t data_capacity)
{
    shader_uniform_group group = (shader_uniform_group){0};
    group.data_capacity = data_capacity;
    group.data = malloc(data_capacity);

    allocate_uniform_list(&group.uniform_list);

    return group;
}

static uniform_data_location*
uniform_group_find(uniform_data_location_list* sentinel, int location)
{
    uniform_data_location* result = 0;
    
    uniform_data_location_list* search = sentinel;
    while(!result && search)
    {
	int search_index;
	int search_count = search->count;
	for(search_index = 0; search_index < search_count; ++search_index)
	{
	    uniform_data_location* search_location =
		(search->locations + search_index);
	    if(search_location->location == location)
	    {
		result = search_location;
		break;
	    }
	}

	search = search->next;
    }

    return result;
}

static uniform_data_location*
uniform_data_location_list_reserve(uniform_data_location_list* sentinel)
{
    uniform_data_location_list* last = sentinel;
    while(last->next) last = last->next;

    if(last->count >= UNIFORM_BLOCK_CAPACITY)
    {
        uniform_data_location_list* add_list =
            (uniform_data_location_list*)malloc(sizeof(uniform_data_location_list));

	allocate_uniform_list(add_list);

        last->next = add_list;
        last = add_list;
    }

    return (last->locations + last->count++);
}

static uniform_data_location*
uniform_group_push(shader_uniform_group* uniform_group, int location, size_t data_size)
{
    uniform_data_location* result = 0;
    size_t data_offset = 0;
    size_t size_after_push = (uniform_group->data_reserved + data_size);
    if(size_after_push <= uniform_group->data_capacity)
    {
        data_offset = uniform_group->data_reserved;
        uniform_group->data_reserved += data_size;

        result = uniform_data_location_list_reserve(&uniform_group->uniform_list);
        result->location = location;
        result->offset = data_offset;
        result->size = data_size;
    }

    return result;
}

void
shader_uniform_set_data(shader_uniform_group* uniform_group, int location,
                        void* data, size_t data_size)
{
    uniform_data_location* data_location =
        uniform_group_find(&uniform_group->uniform_list, location);
    if(!data_location)
    {
        data_location =
            uniform_group_push(uniform_group, location, data_size);
    }
    
    if(data_location && data_size <= data_location->size)
    {
        unsigned char* data_destination =
            (uniform_group->data + data_location->offset);

        platform_copy_memory(data_destination, data, data_size);
    }
}

shader_uniform_data
shader_uniform_get_data(shader_uniform_group* uniform_group, int location)
{
    shader_uniform_data data = (shader_uniform_data){0};
    
    uniform_data_location* data_location =
        uniform_group_find(&uniform_group->uniform_list, location);
    if(data_location)
    {
	data.size = data_location->size;
	data.data = (uniform_group->data + data_location->offset);
    }

    return data;
}
