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
    loaded_mesh mesh = (loaded_mesh){0};
    mesh.data = data;
    
    GLuint* vertex_buffer = 0;
    size_t vertex_buffer_size = 0;
    
    if(data.vertices.positions)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_positions);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);
	
	vertex_buffer_size = (sizeof(vector3) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
		      data.vertices.positions, GL_STATIC_DRAW);

	VA_INCLUDE(mesh.attribute_mask, VA_POSITIONS_BIT);
    }

    if(data.vertices.normals)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_normals);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(vector3) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.normals, GL_STATIC_DRAW);

	VA_INCLUDE(mesh.attribute_mask, VA_NORMALS_BIT);
    }

    if(data.vertices.texcoords)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_texcoords);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(vector2) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.texcoords, GL_STATIC_DRAW);
	
	VA_INCLUDE(mesh.attribute_mask, VA_TEXCOORDS_BIT);
    }

    if(data.vertices.colors)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_colors);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(color) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.colors, GL_STATIC_DRAW);

	VA_INCLUDE(mesh.attribute_mask, VA_COLORS_BIT);
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    return mesh;
}

mesh_data
mesh_create_triangle(float side)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 3;

    float half_side = side * 0.5f;

    size_t position_data_size = (data.vertex_count * sizeof(vector3));
    vector3* positions = (vector3*)malloc(position_data_size);
    *(positions + 0) = (vector3){{{0.0f, half_side, 0.0f}}};
    *(positions + 1) = (vector3){{{-half_side, -half_side, 0.0f}}};
    *(positions + 2) = (vector3){{{half_side, -half_side, 0.0f}}};
    data.vertices.positions = positions;
    
    size_t texcoord_data_size = (data.vertex_count * sizeof(vector2));
    vector2* texcoords = (vector2*)malloc(texcoord_data_size);
    *(texcoords + 0) = (vector2){{{0.5f, 1.0f}}};
    *(texcoords + 1) = (vector2){{{0.0f, 0.0f}}};
    *(texcoords + 2) = (vector2){{{1.0f, 0.0f}}};
    data.vertices.texcoords = texcoords;

    return data;
}

mesh_data
mesh_create_circle(float radius, int subdivisions)
{
    mesh_data data = (mesh_data){0};

    int segments = (subdivisions * 4);
    data.vertex_count = (segments * 3);

    size_t position_data_size = (sizeof(vector3) * data.vertex_count);
    vector3* positions = (vector3*)malloc(position_data_size);
    data.vertices.positions = positions;

    vector3 center = (vector3){{{0.0f, 0.0f, 0.0f}}};

    float segment_step = MATH_PIOVER2 / subdivisions;

    int segment, vertex_index = 0;
    for(segment = 0; segment < subdivisions; ++segment)
    {
	float circle_x1 = cos(segment * segment_step) * radius;
	float circle_y1 = sin(segment * segment_step) * radius;

	float circle_x2 = cos((segment + 1) * segment_step) * radius;
	float circle_y2 = sin((segment + 1) * segment_step) * radius;

	// side 0
	vector3 segment0_0 = (vector3){{{circle_x1, circle_y1, 0.0f}}};
	vector3 segment1_0 = (vector3){{{circle_x2, circle_y2, 0.0f}}};

	*(positions + vertex_index++) = center;
	*(positions + vertex_index++) = segment0_0;
	*(positions + vertex_index++) = segment1_0;

	// side 1
	vector3 segment0_1 = (vector3){{{circle_x1, -circle_y1, 0.0f}}};
	vector3 segment1_1 = (vector3){{{circle_x2, -circle_y2, 0.0f}}};
	
	*(positions + vertex_index++) = center;
	*(positions + vertex_index++) = segment1_1;
	*(positions + vertex_index++) = segment0_1;

	// side 2
	vector3 segment0_2 = (vector3){{{-circle_x1, circle_y1, 0.0f}}};
	vector3 segment1_2 = (vector3){{{-circle_x2, circle_y2, 0.0f}}};

	*(positions + vertex_index++) = center;
	*(positions + vertex_index++) = segment1_2;
	*(positions + vertex_index++) = segment0_2;

	// side 3
	vector3 segment0_3 = (vector3){{{-circle_x1, -circle_y1, 0.0f}}};
	vector3 segment1_3 = (vector3){{{-circle_x2, -circle_y2, 0.0f}}};

	*(positions + vertex_index++) = center;
	*(positions + vertex_index++) = segment0_3;
	*(positions + vertex_index++) = segment1_3;
    }

    return data;
}

#include <string.h>

mesh_data
mesh_create_cube(float side)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 36;

    size_t position_data_size = (sizeof(vector3) * data.vertex_count);
    vector3* positions = (vector3*)malloc(position_data_size);
    data.vertices.positions = positions;
    
    memset(positions, 0, position_data_size);

    int vertex_index = 0;
    float half_side = side * 0.5f;
    
    vector3 side_right[4] = {
	// [0]TOP RIGHT FRONT
	{{{half_side, half_side, half_side}}},
	// [1]TOP RIGHT BACK
	{{{half_side, half_side, -half_side}}},
	// [2]BOTTOM RIGHT FRONT
	{{{half_side, -half_side, half_side}}},
	// [3]BOTTOM RIGHT BACK
	{{{half_side, -half_side, -half_side}}}
    };

    vector3 side_left[4] = {
	// [4]TOP LEFT FRONT
	{{{-half_side, half_side, half_side}}},
	// [5]TOP LEFT BACK
	{{{-half_side, half_side, -half_side}}},
	// [6]BOTTOM LEFT FRONT
	{{{-half_side, -half_side, half_side}}},
	// [7]BOTTOM LEFT BACK
	{{{-half_side, -half_side, -half_side}}}
    };

    vector3 side_top[4] = {
	// [0]TOP RIGHT FRONT
	{{{half_side, half_side, half_side}}},
	// [1]TOP RIGHT BACK
	{{{half_side, half_side, -half_side}}},
	// [4]TOP LEFT FRONT
	{{{-half_side, half_side, half_side}}},
	// [5]TOP LEFT BACK
	{{{-half_side, half_side, -half_side}}}
    };

    vector3 side_bottom[4] = {
	// [2]BOTTOM RIGHT FRONT
	{{{half_side, -half_side, half_side}}},
	// [3]BOTTOM RIGHT BACK
	{{{half_side, -half_side, -half_side}}},
	// [6]BOTTOM LEFT FRONT
	{{{-half_side, -half_side, half_side}}},
	// [7]BOTTOM LEFT BACK
	{{{-half_side, -half_side, -half_side}}}
    };

    vector3 side_front[4] = {
	// [0]TOP RIGHT FRONT
	{{{half_side, half_side, half_side}}},
	// [2]BOTTOM RIGHT FRONT
	{{{half_side, -half_side, half_side}}},
	// [4]TOP LEFT FRONT
	{{{-half_side, half_side, half_side}}},
	// [6]BOTTOM LEFT FRONT
	{{{-half_side, -half_side, half_side}}}
    };

    vector3 side_back[4] = {
	// [1]TOP RIGHT BACK
	{{{half_side, half_side, -half_side}}},
	// [3]BOTTOM RIGHT BACK
	{{{half_side, -half_side, -half_side}}},
	// [5]TOP LEFT BACK
	{{{-half_side, half_side, -half_side}}},
	// [7]BOTTOM LEFT BACK
	{{{-half_side, -half_side, -half_side}}}
    };

    // Right Side
    *(positions + vertex_index++) = *(side_right + 0);
    *(positions + vertex_index++) = *(side_right + 1);
    *(positions + vertex_index++) = *(side_right + 2);
    *(positions + vertex_index++) = *(side_right + 2);
    *(positions + vertex_index++) = *(side_right + 1);
    *(positions + vertex_index++) = *(side_right + 3);

    // Left Side
    *(positions + vertex_index++) = *(side_left + 1);
    *(positions + vertex_index++) = *(side_left + 0);
    *(positions + vertex_index++) = *(side_left + 2);
    *(positions + vertex_index++) = *(side_left + 2);
    *(positions + vertex_index++) = *(side_left + 3);
    *(positions + vertex_index++) = *(side_left + 1);
    
    // Top Side
    *(positions + vertex_index++) = *(side_top + 2);
    *(positions + vertex_index++) = *(side_top + 3);
    *(positions + vertex_index++) = *(side_top + 1);
    *(positions + vertex_index++) = *(side_top + 1);
    *(positions + vertex_index++) = *(side_top + 0);
    *(positions + vertex_index++) = *(side_top + 2);
    
    // Bottom Side
    *(positions + vertex_index++) = *(side_bottom + 2);
    *(positions + vertex_index++) = *(side_bottom + 1);
    *(positions + vertex_index++) = *(side_bottom + 3);
    *(positions + vertex_index++) = *(side_bottom + 1);
    *(positions + vertex_index++) = *(side_bottom + 2);
    *(positions + vertex_index++) = *(side_bottom + 0);

    // Front Side
    *(positions + vertex_index++) = *(side_front + 3);
    *(positions + vertex_index++) = *(side_front + 2);
    *(positions + vertex_index++) = *(side_front + 1);
    *(positions + vertex_index++) = *(side_front + 2);
    *(positions + vertex_index++) = *(side_front + 0);
    *(positions + vertex_index++) = *(side_front + 1);
    
    // Back Side
    *(positions + vertex_index++) = *(side_back + 3);
    *(positions + vertex_index++) = *(side_back + 0);
    *(positions + vertex_index++) = *(side_back + 2);
    *(positions + vertex_index++) = *(side_back + 0);
    *(positions + vertex_index++) = *(side_back + 3);
    *(positions + vertex_index++) = *(side_back + 1);

    return data;
}

mesh_data
mesh_create_plane_xz(float side, int subdivisions)
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = (subdivisions * subdivisions) * 6;

    size_t position_data_size = (sizeof(vector3) * data.vertex_count);
    vector3* positions = (vector3*)malloc(position_data_size);
    data.vertices.positions = positions;
    
    memset(positions, 0, position_data_size);

    float half_side = side * 0.5f;
    float quad_side = side / subdivisions;

    vector3 corners[4] = {
	{{{0.0f, 0.0f, 1.0f}}},
	{{{1.0f, 0.0f, 1.0f}}},
	{{{0.0f, 0.0f, 0.0f}}},
	{{{1.0f, 0.0f, 0.0f}}}
    };

    int vertex_index = 0;
    
    int x, y;
    for(y = 0; y < subdivisions; y++)
    {
	float z0 = (y * quad_side) - half_side;
	float z1 = ((y + 1) * quad_side) - half_side;

	corners[0].z = z1;
	corners[1].z = z1;
	corners[2].z = z0;
	corners[3].z = z0;
	
	for(x = 0; x < subdivisions; x++)
	{
	    float x0 = (x * quad_side) - half_side;
	    float x1 = ((x + 1) * quad_side) - half_side;
	
	    corners[0].x = x0;
	    corners[1].x = x1;
	    corners[2].x = x0;
	    corners[3].x = x1;
	    
	    *(positions + vertex_index++) = *(corners + 2);
	    *(positions + vertex_index++) = *(corners + 3);
	    *(positions + vertex_index++) = *(corners + 0);

	    *(positions + vertex_index++) = *(corners + 0);
	    *(positions + vertex_index++) = *(corners + 3);
	    *(positions + vertex_index++) = *(corners + 1);
	}
    }

    return data;
}

void
mesh_data_free(mesh_data* data)
{
    if(data->vertices.positions)
    {
	free(data->vertices.positions);
	data->vertices.positions = 0;
    }

    if(data->vertices.normals)
    {
	free(data->vertices.normals);
	data->vertices.normals = 0;
    }

    if(data->vertices.texcoords)
    {
	free(data->vertices.texcoords);
	data->vertices.texcoords = 0;
    }

    if(data->vertices.colors)
    {
	free(data->vertices.colors);
	data->vertices.colors = 0;
    }
}

#undef create_vertex
