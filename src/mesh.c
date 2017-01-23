// mesh.c

#include "opengl.h"
#include "mesh.h"

// TODO: Use platform for memory allocation instead of stdlib.
#include <stdlib.h>
#include <math.h>

#include "obj_importer.c"

#define create_vertex(x, y, z, color, uv_x, uv_y) (vertex){{{{x, y, z}}}, color, {{{uv_x, uv_y}}}}

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

    gl->glEnableVertexAttribArray(2);
    gl->glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(sizeof(float) * 6));

    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
    gl->glBindVertexArray(0);

    return mesh;
}

mesh_data
mesh_create_triangle(float side)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 3;

    unsigned int vertex_data_size = data.vertex_count * sizeof(vertex);

    float half_side = side * 0.5f;

    color c1 = { 1.0f, 0.0f, 0.0f };
    color c2 = { 0.0f, 1.0f, 0.0f };
    color c3 = { 0.0f, 0.0f, 1.0f };
    
    data.vertices = (vertex*)malloc(vertex_data_size);
    *(data.vertices + 0) = create_vertex(0.0f, half_side, 0.0f, c1, 0.5f, 1.0f);
    *(data.vertices + 1) = create_vertex(-half_side, -half_side, 0.0f, c2, 0.0f, 0.0f);
    *(data.vertices + 2) = create_vertex(half_side, -half_side, 0.0f, c3, 1.0f, 0.0f);

    return data;
}

mesh_data
mesh_create_circle(float radius, int subdivisions)
{
    mesh_data data = (mesh_data){0};

    int segments = (subdivisions * 4);
    int vertex_count = (segments * 3);

    unsigned int vertex_data_size = vertex_count * sizeof(vertex);
    data.vertices = (vertex*)malloc(vertex_data_size);
    data.vertex_count = vertex_count;

    color fill_color = { 1.0f, 1.0f, 1.0f };
    vertex center = create_vertex(0.0f, 0.0f, 0.0f, fill_color, 0.0f, 0.0f);

    float segment_step = MATH_PIOVER2 / subdivisions;

    int segment, vertex_index = 0;
    for(segment = 0; segment < subdivisions; ++segment)
    {
	float circle_x1 = cos(segment * segment_step) * radius;
	float circle_y1 = sin(segment * segment_step) * radius;

	float circle_x2 = cos((segment + 1) * segment_step) * radius;
	float circle_y2 = sin((segment + 1) * segment_step) * radius;

	// side 0
	vertex segment0_0 = create_vertex(circle_x1, circle_y1, 0.0f,
					  fill_color, circle_x1, circle_y1);
	vertex segment1_0 = create_vertex(circle_x2, circle_y2, 0.0f,
					  fill_color, circle_x1, circle_y1);

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment0_0;
	*(data.vertices + vertex_index++) = segment1_0;

	// side 1
	vertex segment0_1 = create_vertex(circle_x1, -circle_y1, 0.0f,
					  fill_color, circle_x1, -circle_y1);
	vertex segment1_1 = create_vertex(circle_x2, -circle_y2, 0.0f,
					  fill_color, circle_x1, -circle_y2);
	
	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment1_1;
	*(data.vertices + vertex_index++) = segment0_1;

	// side 2
	vertex segment0_2 = create_vertex(-circle_x1, circle_y1, 0.0f,
					  fill_color, -circle_x1, circle_y1);
	vertex segment1_2 = create_vertex(-circle_x2, circle_y2, 0.0f,
					  fill_color, -circle_x1, circle_y2);

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment1_2;
	*(data.vertices + vertex_index++) = segment0_2;

	// side 3
	vertex segment0_3 = create_vertex(-circle_x1, -circle_y1, 0.0f,
					  fill_color, -circle_x1, -circle_y1);
	vertex segment1_3 = create_vertex(-circle_x2, -circle_y2, 0.0f,
					  fill_color, -circle_x2, -circle_y2);

	*(data.vertices + vertex_index++) = center;
	*(data.vertices + vertex_index++) = segment0_3;
	*(data.vertices + vertex_index++) = segment1_3;
    }

    return data;
}

#include <string.h>

mesh_data
mesh_create_cube(float side, color vertex_color)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 36;

    unsigned int vertex_data_size = data.vertex_count * sizeof(vertex);

    float half_side = side * 0.5f;
    
    data.vertices = (vertex*)malloc(vertex_data_size);
    memset(data.vertices, 0, vertex_data_size);

    int vertex_index = 0;

    // NOTE: Separated into each side for UV reasons. In the future I will consider
    // separating the vertex, color, and UV into different streams.
    
    vertex side_right[4] = {
	// [0]TOP RIGHT FRONT
	{{{{half_side, half_side, half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
	// [1]TOP RIGHT BACK
	{{{{half_side, half_side, -half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [2]BOTTOM RIGHT FRONT
	{{{{half_side, -half_side, half_side}}}, vertex_color, {{{0.0f, 0.0f}}}},
	// [3]BOTTOM RIGHT BACK
	{{{{half_side, -half_side, -half_side}}}, vertex_color, {{{1.0f, 0.0f}}}},
    };

    vertex side_left[4] = {
	// [4]TOP LEFT FRONT
	{{{{-half_side, half_side, half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [5]TOP LEFT BACK
	{{{{-half_side, half_side, -half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
	// [6]BOTTOM LEFT FRONT
	{{{{-half_side, -half_side, half_side}}}, vertex_color, {{{1.0f, 0.0f}}}},
	// [7]BOTTOM LEFT BACK
	{{{{-half_side, -half_side, -half_side}}}, vertex_color, {{{0.0f, 0.0f}}}}
    };

    vertex side_top[4] = {
	// [0]TOP RIGHT FRONT
	{{{{half_side, half_side, half_side}}}, vertex_color, {{{1.0f, 0.0f}}}},
	// [1]TOP RIGHT BACK
	{{{{half_side, half_side, -half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [4]TOP LEFT FRONT
	{{{{-half_side, half_side, half_side}}}, vertex_color, {{{0.0f, 0.0f}}}},
	// [5]TOP LEFT BACK
	{{{{-half_side, half_side, -half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
    };

    vertex side_bottom[4] = {
	// [2]BOTTOM RIGHT FRONT
	{{{{half_side, -half_side, half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [3]BOTTOM RIGHT BACK
	{{{{half_side, -half_side, -half_side}}}, vertex_color, {{{1.0f, 0.0f}}}},
	// [6]BOTTOM LEFT FRONT
	{{{{-half_side, -half_side, half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
	// [7]BOTTOM LEFT BACK
	{{{{-half_side, -half_side, -half_side}}}, vertex_color, {{{0.0f, 0.0f}}}}
    };

    vertex side_front[4] = {
	// [0]TOP RIGHT FRONT
	{{{{half_side, half_side, half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [2]BOTTOM RIGHT FRONT
	{{{{half_side, -half_side, half_side}}}, vertex_color, {{{1.0f, 0.0f}}}},
	// [4]TOP LEFT FRONT
	{{{{-half_side, half_side, half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
	// [6]BOTTOM LEFT FRONT
	{{{{-half_side, -half_side, half_side}}}, vertex_color, {{{0.0f, 0.0f}}}},
    };

    vertex side_back[4] = {
	// [1]TOP RIGHT BACK
	{{{{half_side, half_side, -half_side}}}, vertex_color, {{{0.0f, 1.0f}}}},
	// [3]BOTTOM RIGHT BACK
	{{{{half_side, -half_side, -half_side}}}, vertex_color, {{{0.0f, 0.0f}}}},
	// [5]TOP LEFT BACK
	{{{{-half_side, half_side, -half_side}}}, vertex_color, {{{1.0f, 1.0f}}}},
	// [7]BOTTOM LEFT BACK
	{{{{-half_side, -half_side, -half_side}}}, vertex_color, {{{1.0f, 0.0f}}}}
    };

    // Right Side
    *(data.vertices + vertex_index++) = *(side_right + 0);
    *(data.vertices + vertex_index++) = *(side_right + 1);
    *(data.vertices + vertex_index++) = *(side_right + 2);
    *(data.vertices + vertex_index++) = *(side_right + 2);
    *(data.vertices + vertex_index++) = *(side_right + 1);
    *(data.vertices + vertex_index++) = *(side_right + 3);

    // Left Side
    *(data.vertices + vertex_index++) = *(side_left + 1);
    *(data.vertices + vertex_index++) = *(side_left + 0);
    *(data.vertices + vertex_index++) = *(side_left + 2);
    *(data.vertices + vertex_index++) = *(side_left + 2);
    *(data.vertices + vertex_index++) = *(side_left + 3);
    *(data.vertices + vertex_index++) = *(side_left + 1);
    
    // Top Side
    *(data.vertices + vertex_index++) = *(side_top + 2);
    *(data.vertices + vertex_index++) = *(side_top + 3);
    *(data.vertices + vertex_index++) = *(side_top + 1);
    *(data.vertices + vertex_index++) = *(side_top + 1);
    *(data.vertices + vertex_index++) = *(side_top + 0);
    *(data.vertices + vertex_index++) = *(side_top + 2);
    
    // Bottom Side
    *(data.vertices + vertex_index++) = *(side_bottom + 2);
    *(data.vertices + vertex_index++) = *(side_bottom + 1);
    *(data.vertices + vertex_index++) = *(side_bottom + 3);
    *(data.vertices + vertex_index++) = *(side_bottom + 1);
    *(data.vertices + vertex_index++) = *(side_bottom + 2);
    *(data.vertices + vertex_index++) = *(side_bottom + 0);

    // Front Side
    *(data.vertices + vertex_index++) = *(side_front + 3);
    *(data.vertices + vertex_index++) = *(side_front + 2);
    *(data.vertices + vertex_index++) = *(side_front + 1);
    *(data.vertices + vertex_index++) = *(side_front + 2);
    *(data.vertices + vertex_index++) = *(side_front + 0);
    *(data.vertices + vertex_index++) = *(side_front + 1);
    
    // Back Side
    *(data.vertices + vertex_index++) = *(side_back + 3);
    *(data.vertices + vertex_index++) = *(side_back + 0);
    *(data.vertices + vertex_index++) = *(side_back + 2);
    *(data.vertices + vertex_index++) = *(side_back + 0);
    *(data.vertices + vertex_index++) = *(side_back + 3);
    *(data.vertices + vertex_index++) = *(side_back + 1);

    return data;
}

mesh_data
mesh_create_plane_xz(float side, int subdivisions, color vertex_color)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = (subdivisions * subdivisions) * 6;

    unsigned int vertex_data_size = data.vertex_count * sizeof(vertex);
    
    data.vertices = (vertex*)malloc(vertex_data_size);
    memset(data.vertices, 0, vertex_data_size);

    float half_side = side * 0.5f;
    float quad_side = side / subdivisions;

    vertex corners[4] = {
	{{{{0.0f, 0.0f, 1.0f}}}, vertex_color, {{{0.0f, 1.0f}}}},
	{{{{1.0f, 0.0f, 1.0f}}}, vertex_color, {{{1.0f, 1.0f}}}},
	{{{{0.0f, 0.0f, 0.0f}}}, vertex_color, {{{0.0f, 0.0f}}}},
	{{{{1.0f, 0.0f, 0.0f}}}, vertex_color, {{{1.0f, 0.0f}}}},
    };

    int vertex_index = 0;
    
    int x, y;
    for(y = 0; y < subdivisions; y++)
    {
	float z0 = (y * quad_side) - half_side;
	float z1 = ((y + 1) * quad_side) - half_side;

	corners[0].p.z = z1;
	corners[1].p.z = z1;
	corners[2].p.z = z0;
	corners[3].p.z = z0;
	
	for(x = 0; x < subdivisions; x++)
	{
	    float x0 = (x * quad_side) - half_side;
	    float x1 = ((x + 1) * quad_side) - half_side;
	
	    corners[0].p.x = x0;
	    corners[1].p.x = x1;
	    corners[2].p.x = x0;
	    corners[3].p.x = x1;
	    
	    *(data.vertices + vertex_index++) = *(corners + 2);
	    *(data.vertices + vertex_index++) = *(corners + 3);
	    *(data.vertices + vertex_index++) = *(corners + 0);

	    *(data.vertices + vertex_index++) = *(corners + 0);
	    *(data.vertices + vertex_index++) = *(corners + 3);
	    *(data.vertices + vertex_index++) = *(corners + 1);
	}
    }

    return data;
}

void
mesh_data_free(mesh_data* data)
{
    if(data->vertices)
    {
	free(data->vertices);
	data->vertices = 0;
    }
}

#undef create_vertex
