#ifndef MESH_H_INCLUDED
#define MESH_H_INCLUDED

typedef struct
{
    float x;
    float y;
    float z;
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
void mesh_data_free(mesh_data* data);

#endif // MESH_H_INCLUDED
