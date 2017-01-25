#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "math.h"

#define MESH_MAX_VERTEX_BUFFERS 8
#define MESH_MAX_INPUT_LAYOUTS 16

typedef enum vertex_attributes vertex_attributes;

enum vertex_attributes
{
    vertex_attributes_positions,
    vertex_attributes_normals,
    vertex_attributes_texcoords,
    vertex_attributes_colors,
    vertex_attributes_count
};

#define VA_POSITIONS_BIT (1 << vertex_attributes_positions)
#define VA_NORMALS_BIT   (1 << vertex_attributes_normals)
#define VA_TEXCOORDS_BIT (1 << vertex_attributes_texcoords)
#define VA_COLORS_BIT    (1 << vertex_attributes_colors)

#define VA_ISSET(mask, bits) ((mask & bits) == bits)
#define VA_INCLUDE(mask, bits) (mask |= bits)
#define VA_EXCLUDE(mask, bits) (mask ^= bits)

typedef struct color color;
typedef struct vertex_data vertex_data;
typedef struct mesh_data mesh_data;
typedef struct input_layout input_layout;
typedef struct loaded_mesh loaded_mesh;

struct color
{
    float r;
    float g;
    float b;
};

struct vertex_data
{
    vector3* positions;
    vector3* normals;
    vector2* texcoords;
    color* colors;
};

struct mesh_data
{
    int vertex_count;
    vertex_data vertices;
};

struct input_layout
{
    int attribute_mask;
    GLuint vertex_array;
};

struct loaded_mesh
{
    int attribute_mask;
    int layout_count;
    input_layout layouts[MESH_MAX_INPUT_LAYOUTS];
    GLuint vertex_buffer[MESH_MAX_VERTEX_BUFFERS];
    mesh_data data;
};

loaded_mesh load_mesh(gl_functions* gl, mesh_data data);

mesh_data mesh_create_triangle(float side);
mesh_data mesh_create_circle(float radius, int subdivisions);
mesh_data mesh_create_cube(float side);
mesh_data mesh_create_plane_xz(float side, int subdivisions);
void mesh_data_free(mesh_data* data);

#endif // MESH_H_INCLUDED
