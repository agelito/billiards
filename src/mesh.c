// mesh.c

#include "gl_extensions.h"
#include "mesh.h"

// TODO: Use platform for memory allocation instead of stdlib.
#include <stdlib.h>
#include <math.h>

#define MATH_PI 3.14159265359f
#define MATH_PIOVER2 1.57079632679f

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
    gl->glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), NULL);

    gl->glEnableVertexAttribArray(1);
    gl->glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(float) * 3));

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

    color c = { 0.0f, 0.0f, 1.0f };
    
    data.vertices = (vertex*)malloc(vertex_data_size);
    *(data.vertices + 0) = (vertex){{0.0f, half_side, 0.0f}, c};
    *(data.vertices + 1) = (vertex){{-half_side, -half_side, 0.0f}, c};
    *(data.vertices + 2) = (vertex){{half_side, -half_side, 0.0f}, c};

    return data;
}

mesh_data mesh_create_circle(float radius, int segments_per_side)
{
    mesh_data data = (mesh_data){0};

    int segments = (segments_per_side * 4);
    int vertex_count = (segments * 3);

    unsigned int vertex_data_size = vertex_count * sizeof(vertex);
    data.vertices = (vertex*)malloc(vertex_data_size);
    data.vertex_count = vertex_count;

    color color_center = { 0.5f, 0.5f, 0.5f };
    color color_outer = { 0.08f, 0.08f, 0.08f };
    vertex center = (vertex){{0.0f, 0.0f, 0.0f}, color_center};

    float segment_step = MATH_PIOVER2 / segments_per_side;

    int segment, vertex_index = 0;
    for(segment = 0; segment < segments_per_side; ++segment)
    {
	float circle_x1 = cos(segment * segment_step) * radius;
	float circle_y1 = sin(segment * segment_step) * radius;

	float circle_x2 = cos((segment + 1) * segment_step) * radius;
	float circle_y2 = sin((segment + 1) * segment_step) * radius;

	// side 0
	vertex segment0_0 = (vertex){{circle_x1, circle_y1, 0.0f}, color_outer};
	vertex segment1_0 = (vertex){{circle_x2, circle_y2, 0.0f}, color_outer};

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment0_0;
	*(data.vertices + vertex_index++) = segment1_0;

	// side 1
	vertex segment0_1 = (vertex){{circle_x1, -circle_y1, 0.0f}, color_outer};
	vertex segment1_1 = (vertex){{circle_x2, -circle_y2, 0.0f}, color_outer};
	
	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment1_1;
	*(data.vertices + vertex_index++) = segment0_1;

	// side 2
	vertex segment0_2 = (vertex){{-circle_x1, circle_y1, 0.0f}, color_outer};
	vertex segment1_2 = (vertex){{-circle_x2, circle_y2, 0.0f}, color_outer};

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment1_2;
	*(data.vertices + vertex_index++) = segment0_2;

	// side 3
	vertex segment0_3 = (vertex){{-circle_x1, -circle_y1, 0.0f}, color_outer};
	vertex segment1_3 = (vertex){{-circle_x2, -circle_y2, 0.0f}, color_outer};

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment0_3;
	*(data.vertices + vertex_index++) = segment1_3;
    }

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
