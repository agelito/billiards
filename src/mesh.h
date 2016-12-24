#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

#include "math.h"

typedef struct
{
    float r;
    float g;
    float b;
} color;

typedef struct
{
    vector3 p;
    color c;
} vertex;

typedef struct
{
    int vertex_count;
    vertex* vertices;
} mesh_data;

typedef struct
{
    GLuint vao;
    GLuint vbo;
    mesh_data data;
} loaded_mesh;

loaded_mesh load_mesh(gl_functions* gl, mesh_data data);

mesh_data mesh_create_triangle(float side);
mesh_data mesh_create_circle(float radius, int subdivisions);
mesh_data mesh_create_cube(float side);
void mesh_data_free(mesh_data* data);

#endif // MESH_H_INCLUDED
