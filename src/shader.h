#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

typedef struct shader_uniform shader_uniform;
typedef struct shader_reflection shader_reflection;
typedef struct shader_uniform_data shader_uniform_data;

typedef enum
{
    shader_data_float1,
    shader_data_float2,
    shader_data_float3,
    shader_data_float4,
    shader_data_integer1,
    shader_data_integer2,
    shader_data_integer3,
    shader_data_integer4,
    shader_data_matrix2,
    shader_data_matrix3,
    shader_data_matrix4,
    shader_data_unknown,
} shader_data_type;

struct shader_uniform
{
    char* name;
    int location;
    size_t size_per_element;
    shader_data_type type;
};

struct shader_uniform_data
{
    size_t size;
    unsigned char* data;
};

struct shader_reflection
{
    int uniform_count;
    shader_uniform* uniforms;
};

typedef struct
{
    GLuint vertex;
    GLuint fragment;
    GLuint program;
    shader_reflection info;
} shader_program;

typedef struct uniform_data_location_list
{
    int count;
    struct uniform_data_location* locations;
    struct uniform_data_location_list* next;
} uniform_data_location_list;

typedef struct
{
    uniform_data_location_list uniform_list;
    
    size_t data_capacity;
    size_t data_reserved;
    unsigned char* data;
} shader_uniform_group;

shader_program
load_shader(gl_functions* gl, char* vertex_source, int vertex_source_length,
	    char* fragment_source, int fragment_source_length);

size_t
shader_data_type_size(shader_data_type type, int count);

shader_reflection
shader_reflect(gl_functions* gl, shader_program* shader);

shader_uniform*
shader_get_uniform(shader_reflection* reflection, char* uniform_name);

shader_uniform_group
shader_uniform_group_create(size_t data_capacity);

void
shader_uniform_set_data(shader_uniform_group* uniform_group, int location,
			 void* data, size_t data_size);

shader_uniform_data
shader_uniform_get_data(shader_uniform_group* uniform_group, int location);

#endif // SHADER_H_INCLUDED
