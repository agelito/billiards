// mesh.c

#include "gl_extensions.h"
#include "mesh.h"

// TODO: Use platform for memory allocation instead of stdlib.
#include <stdlib.h>

loaded_mesh
load_mesh(gl_functions* gl, mesh_data data)
{
    loaded_mesh mesh;
    
    GLuint vao;
    gl->glGenVertexArrays(1, &vao);
    gl->glBindVertexArray(vao);
    mesh.vao = vao;

    GLuint vbo;
    gl->glGenBuffers(1, &vbo);
    gl->glBindBuffer(GL_ARRAY_BUFFER, vbo);
    mesh.vbo = vbo;

    mesh.data = data;

    unsigned int vertex_data_size = data.vertex_count * sizeof(vertex);
    gl->glBufferData(GL_ARRAY_BUFFER, vertex_data_size, data.vertices, GL_STATIC_DRAW);

    // TODO: Vertex attribute bind point hardcoded.
    gl->glEnableVertexAttribArray(0);
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl->glBindVertexArray(0);

    return mesh;
}

mesh_data mesh_create_triangle(float side)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 3;

    unsigned int vertex_data_size = data.vertex_count * sizeof(vertex);

    float half_side = side * 0.5f;
    
    data.vertices = (vertex*)malloc(vertex_data_size);
    *(data.vertices + 0) = (vertex){0.0f, half_side, 0.0f};
    *(data.vertices + 1) = (vertex){-half_side, -half_side, 0.0f};
    *(data.vertices + 2) = (vertex){half_side, -half_side, 0.0f};

    return data;
}

void mesh_data_free(mesh_data* data)
{
    if(data->vertices)
    {
	free(data->vertices);
	data->vertices = 0;
    }
}
