#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "math.h"

#define MESH_MAX_VERTEX_BUFFERS 8

typedef struct
{
    float r;
    float g;
    float b;
} color;

typedef struct
{
    vector3* positions;
    vector3* normals;
    vector2* texcoords;
    color* colors;
} vertex_data;

typedef struct
{
    int vertex_count;
    vertex_data vertices;
} mesh_data;

typedef struct
{
    GLuint vertex_array;
    GLuint vertex_buffer[MESH_MAX_VERTEX_BUFFERS];
    mesh_data data;
} loaded_mesh;

loaded_mesh load_mesh(gl_functions* gl, mesh_data data);

mesh_data mesh_create_triangle(float side);
mesh_data mesh_create_circle(float radius, int subdivisions);
mesh_data mesh_create_cube(float side);
mesh_data mesh_create_plane_xz(float side, int subdivisions);
void mesh_data_free(mesh_data* data);

#endif // MESH_H_INCLUDED
