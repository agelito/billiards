// mesh.c

#include "opengl.h"
#include "mesh.h"

// TODO: Use platform for memory allocation instead of stdlib.
#include <stdlib.h>
#include <math.h>

#include "../asset_import/obj_importer.c"

#define create_vertex(x, y, z, color, uv_x, uv_y) (vertex){{{{x, y, z}}}, color, {{{uv_x, uv_y}}}}

loaded_mesh
load_mesh(gl_functions* gl, mesh_data data, bool32 dynamic)
{
    loaded_mesh mesh = (loaded_mesh){0};
    mesh.data = data;
    
    GLuint* vertex_buffer = 0;
    size_t vertex_buffer_size = 0;

    GLenum usage = GL_STATIC_DRAW;

    if(dynamic)
    {
	usage = GL_DYNAMIC_DRAW;
    }
    
    if(data.vertices.positions)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_positions);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);
	
	vertex_buffer_size = (sizeof(vector3) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
		      data.vertices.positions, usage);

	VA_INCLUDE(mesh.attribute_mask, VA_POSITIONS_BIT);
    }

    if(data.vertices.normals)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_normals);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(vector3) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.normals, usage);

	VA_INCLUDE(mesh.attribute_mask, VA_NORMALS_BIT);
    }

    if(data.vertices.texcoords)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_texcoords);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(vector2) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.texcoords, usage);
	
	VA_INCLUDE(mesh.attribute_mask, VA_TEXCOORDS_BIT);
    }

    if(data.vertices.colors)
    {
	vertex_buffer = (mesh.vertex_buffer + vertex_attributes_colors);
	gl->glGenBuffers(1, vertex_buffer);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_size = (sizeof(color) * data.vertex_count);
	gl->glBufferData(GL_ARRAY_BUFFER, vertex_buffer_size,
			  data.vertices.colors, usage);

	VA_INCLUDE(mesh.attribute_mask, VA_COLORS_BIT);
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);

    return mesh;
}

void
update_mesh(gl_functions* gl, loaded_mesh* mesh, uint32 offset, uint32 count)
{
    GLuint* vertex_buffer = 0;
    size_t vertex_buffer_offset = 0;;
    size_t vertex_buffer_size = 0;
    
    if(VA_ISSET(mesh->attribute_mask, VA_POSITIONS_BIT))
    {
	vertex_buffer = (mesh->vertex_buffer + vertex_attributes_positions);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_offset = (offset * sizeof(vector3));
	vertex_buffer_size = (count * sizeof(vector3));
	gl->glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_offset, vertex_buffer_size,
			    mesh->data.vertices.positions);
    }

    if(VA_ISSET(mesh->attribute_mask, VA_NORMALS_BIT))
    {
	vertex_buffer = (mesh->vertex_buffer + vertex_attributes_normals);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_offset = (offset * sizeof(vector3));
	vertex_buffer_size = (count * sizeof(vector3));
	gl->glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_offset, vertex_buffer_size,
			    mesh->data.vertices.normals);
    }

    if(VA_ISSET(mesh->attribute_mask, VA_TEXCOORDS_BIT))
    {
	vertex_buffer = (mesh->vertex_buffer + vertex_attributes_texcoords);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_offset = (offset * sizeof(vector2));
	vertex_buffer_size = (count * sizeof(vector2));
	gl->glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_offset, vertex_buffer_size,
			    mesh->data.vertices.texcoords);
    }

    if(VA_ISSET(mesh->attribute_mask, VA_COLORS_BIT))
    {
	vertex_buffer = (mesh->vertex_buffer + vertex_attributes_colors);
	gl->glBindBuffer(GL_ARRAY_BUFFER, *vertex_buffer);

	vertex_buffer_offset = (offset * sizeof(vector3));
	vertex_buffer_size = (count * sizeof(vector3));
	gl->glBufferSubData(GL_ARRAY_BUFFER, vertex_buffer_offset, vertex_buffer_size,
			    mesh->data.vertices.colors);
    }

    gl->glBindBuffer(GL_ARRAY_BUFFER, 0);
}

mesh_data
mesh_create_quad()
{
    mesh_data data = (mesh_data){0};
    data.vertex_count = 6;

    real32 half_side = 0.5f;

    size_t position_data_size = (data.vertex_count * sizeof(vector3));
    vector3* positions = (vector3*)malloc(position_data_size);
    *(positions + 0) = vector3_create(-half_side, -half_side, 0.0f);
    *(positions + 1) = vector3_create(half_side, -half_side, 0.0f);
    *(positions + 2) = vector3_create(-half_side, half_side, 0.0f);
    *(positions + 3) = vector3_create(-half_side, half_side, 0.0f);
    *(positions + 4) = vector3_create(half_side, -half_side, 0.0f);
    *(positions + 5) = vector3_create(half_side, half_side, 0.0f);
    data.vertices.positions = positions;

    size_t texcoord_data_size = (data.vertex_count * sizeof(vector2));
    vector2* texcoords = (vector2*)malloc(texcoord_data_size);
    *(texcoords + 0) = vector2_create(0.0f, 0.0f);
    *(texcoords + 1) = vector2_create(1.0f, 0.0f);
    *(texcoords + 2) = vector2_create(0.0f, 1.0f);
    *(texcoords + 3) = vector2_create(0.0f, 1.0f);
    *(texcoords + 4) = vector2_create(1.0f, 0.0f);
    *(texcoords + 5) = vector2_create(1.0f, 1.0f);
    data.vertices.texcoords = texcoords;

    return data;
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

    size_t texcoord_data_size = (sizeof(vector2) * data.vertex_count);
    vector2* texcoords = (vector2*)malloc(texcoord_data_size);
    data.vertices.texcoords = texcoords;

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
	vector3 segment0_0 = vector3_create(circle_x1, circle_y1, 0.0f);
	vector3 segment1_0 = vector3_create(circle_x2, circle_y2, 0.0f);

	*(positions + vertex_index+0) = center;
	*(positions + vertex_index+1) = segment0_0;
	*(positions + vertex_index+2) = segment1_0;

	*(texcoords + vertex_index+0) = vector2_create(0.5f, 0.5f);
	*(texcoords + vertex_index+1) =
	    vector2_scale(vector2_create(1.0f + circle_x1, 1.0f + circle_y1), 0.5f);
	*(texcoords + vertex_index+2) =
	    vector2_scale(vector2_create(1.0f + circle_x2, 1.0f + circle_y2), 0.5f);
	vertex_index += 3;

	// side 1
	vector3 segment0_1 = vector3_create(circle_x1, -circle_y1, 0.0f);
	vector3 segment1_1 = vector3_create(circle_x2, -circle_y2, 0.0f);
	
	*(positions + vertex_index+0) = center;
	*(positions + vertex_index+1) = segment1_1;
	*(positions + vertex_index+2) = segment0_1;

	*(texcoords + vertex_index+0) = vector2_create(0.5f, 0.5f);
	*(texcoords + vertex_index+2) =
	    vector2_scale(vector2_create(1.0f + circle_x1, 1.0f + -circle_y1), 0.5f);
	*(texcoords + vertex_index+1) =
	    vector2_scale(vector2_create(1.0f + circle_x2, 1.0f + -circle_y2), 0.5f);
	vertex_index += 3;

	// side 2
	vector3 segment0_2 = vector3_create(-circle_x1, circle_y1, 0.0f);
	vector3 segment1_2 = vector3_create(-circle_x2, circle_y2, 0.0f);

	*(positions + vertex_index+0) = center;
	*(positions + vertex_index+1) = segment1_2;
	*(positions + vertex_index+2) = segment0_2;

	*(texcoords + vertex_index+0) = vector2_create(0.5f, 0.5f);
	*(texcoords + vertex_index+2) =
	    vector2_scale(vector2_create(1.0f + -circle_x1, 1.0f + circle_y1), 0.5f);
	*(texcoords + vertex_index+1) =
	    vector2_scale(vector2_create(1.0f + -circle_x2, 1.0f + circle_y2), 0.5f);
	vertex_index += 3;

	// side 3
	vector3 segment0_3 = vector3_create(-circle_x1, -circle_y1, 0.0f);
	vector3 segment1_3 = vector3_create(-circle_x2, -circle_y2, 0.0f);

	*(positions + vertex_index+0) = center;
	*(positions + vertex_index+1) = segment0_3;
	*(positions + vertex_index+2) = segment1_3;

	*(texcoords + vertex_index+0) = vector2_create(0.5f, 0.5f);
	*(texcoords + vertex_index+1) =
	    vector2_scale(vector2_create(1.0f + -circle_x1, 1.0f + -circle_y1), 0.5f);
	*(texcoords + vertex_index+2) =
	    vector2_scale(vector2_create(1.0f + -circle_x2, 1.0f + -circle_y2), 0.5f);
	vertex_index += 3;
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

    size_t texcoord_data_size = (sizeof(vector2) * data.vertex_count);
    vector2* texcoords = (vector2*)malloc(texcoord_data_size);
    data.vertices.texcoords = texcoords;
    
    memset(positions, 0, position_data_size);

    int vertex_index = 0;
    float half_side = side * 0.5f;

    vector2 uv_quad[4] = {
	{{{0.0f, 0.0f}}}, // BOTTOM LEFT
	{{{0.0f, 1.0}}},  // TOP LEFT
	{{{1.0f, 1.0f}}}, // TOP RIGHT
	{{{1.0f, 0.0f}}}  // BOTTOM RIGHT
    };

    vector3 sides[8] = {
	// [0]TOP RIGHT FRONT
	{{{half_side, half_side, half_side}}},
	// [1]TOP RIGHT BACK
	{{{half_side, half_side, -half_side}}},
	// [2]BOTTOM RIGHT FRONT
	{{{half_side, -half_side, half_side}}},
	// [3]BOTTOM RIGHT BACK
	{{{half_side, -half_side, -half_side}}},
	// [4]TOP LEFT FRONT
	{{{-half_side, half_side, half_side}}},
	// [5]TOP LEFT BACK
	{{{-half_side, half_side, -half_side}}},
	// [6]BOTTOM LEFT FRONT
	{{{-half_side, -half_side, half_side}}},
	// [7]BOTTOM LEFT BACK
	{{{-half_side, -half_side, -half_side}}}
    };

    // Right Side
    *(positions + vertex_index+0) = *(sides + 0);
    *(positions + vertex_index+1) = *(sides + 1);
    *(positions + vertex_index+2) = *(sides + 2);
    *(positions + vertex_index+3) = *(sides + 2);
    *(positions + vertex_index+4) = *(sides + 1);
    *(positions + vertex_index+5) = *(sides + 3);

    *(texcoords + vertex_index+0) = *(uv_quad + 1);
    *(texcoords + vertex_index+1) = *(uv_quad + 2);
    *(texcoords + vertex_index+2) = *(uv_quad + 0);
    *(texcoords + vertex_index+3) = *(uv_quad + 0);
    *(texcoords + vertex_index+4) = *(uv_quad + 2);
    *(texcoords + vertex_index+5) = *(uv_quad + 3);

    vertex_index += 6;

    // Left Side
    *(positions + vertex_index+0) = *(sides + 5);
    *(positions + vertex_index+1) = *(sides + 4);
    *(positions + vertex_index+2) = *(sides + 6);
    *(positions + vertex_index+3) = *(sides + 6);
    *(positions + vertex_index+4) = *(sides + 7);
    *(positions + vertex_index+5) = *(sides + 5);
    
    *(texcoords + vertex_index+0) = *(uv_quad + 1);
    *(texcoords + vertex_index+1) = *(uv_quad + 2);
    *(texcoords + vertex_index+2) = *(uv_quad + 3);
    *(texcoords + vertex_index+3) = *(uv_quad + 3);
    *(texcoords + vertex_index+4) = *(uv_quad + 0);
    *(texcoords + vertex_index+5) = *(uv_quad + 1);

    vertex_index += 6;
    
    // Top Side
    *(positions + vertex_index+0) = *(sides + 4);
    *(positions + vertex_index+1) = *(sides + 5);
    *(positions + vertex_index+2) = *(sides + 1);
    *(positions + vertex_index+3) = *(sides + 1);
    *(positions + vertex_index+4) = *(sides + 0);
    *(positions + vertex_index+5) = *(sides + 4);

    *(texcoords + vertex_index+0) = *(uv_quad + 1);
    *(texcoords + vertex_index+1) = *(uv_quad + 0);
    *(texcoords + vertex_index+2) = *(uv_quad + 3);
    *(texcoords + vertex_index+3) = *(uv_quad + 3);
    *(texcoords + vertex_index+4) = *(uv_quad + 2);
    *(texcoords + vertex_index+5) = *(uv_quad + 1);

    vertex_index += 6;
    
    // Bottom Side
    *(positions + vertex_index+0) = *(sides + 6);
    *(positions + vertex_index+1) = *(sides + 3);
    *(positions + vertex_index+2) = *(sides + 7);
    *(positions + vertex_index+3) = *(sides + 3);
    *(positions + vertex_index+4) = *(sides + 6);
    *(positions + vertex_index+5) = *(sides + 2);

    *(texcoords + vertex_index+0) = *(uv_quad + 0);
    *(texcoords + vertex_index+1) = *(uv_quad + 2);
    *(texcoords + vertex_index+2) = *(uv_quad + 1);
    *(texcoords + vertex_index+3) = *(uv_quad + 2);
    *(texcoords + vertex_index+4) = *(uv_quad + 0);
    *(texcoords + vertex_index+5) = *(uv_quad + 3);

    vertex_index += 6;

    // Front Side
    *(positions + vertex_index+0) = *(sides + 6);
    *(positions + vertex_index+1) = *(sides + 4);
    *(positions + vertex_index+2) = *(sides + 2);
    *(positions + vertex_index+3) = *(sides + 4);
    *(positions + vertex_index+4) = *(sides + 0);
    *(positions + vertex_index+5) = *(sides + 2);

    *(texcoords + vertex_index+0) = *(uv_quad + 3);
    *(texcoords + vertex_index+1) = *(uv_quad + 2);
    *(texcoords + vertex_index+2) = *(uv_quad + 0);
    *(texcoords + vertex_index+3) = *(uv_quad + 2);
    *(texcoords + vertex_index+4) = *(uv_quad + 1);
    *(texcoords + vertex_index+5) = *(uv_quad + 0);

    vertex_index += 6;
    
    // Back Side
    *(positions + vertex_index+0) = *(sides + 7);
    *(positions + vertex_index+1) = *(sides + 1);
    *(positions + vertex_index+2) = *(sides + 5);
    *(positions + vertex_index+3) = *(sides + 1);
    *(positions + vertex_index+4) = *(sides + 7);
    *(positions + vertex_index+5) = *(sides + 3);

    *(texcoords + vertex_index+0) = *(uv_quad + 0);
    *(texcoords + vertex_index+1) = *(uv_quad + 2);
    *(texcoords + vertex_index+2) = *(uv_quad + 1);
    *(texcoords + vertex_index+3) = *(uv_quad + 2);
    *(texcoords + vertex_index+4) = *(uv_quad + 0);
    *(texcoords + vertex_index+5) = *(uv_quad + 3);

    vertex_index += 6;

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

    size_t texcoord_data_size = (sizeof(vector2) * data.vertex_count);
    vector2* texcoords = (vector2*)malloc(texcoord_data_size);
    data.vertices.texcoords = texcoords;

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
	    
	    *(positions + vertex_index+0) = *(corners + 2);
	    *(positions + vertex_index+1) = *(corners + 3);
	    *(positions + vertex_index+2) = *(corners + 0);
	    *(positions + vertex_index+3) = *(corners + 0);
	    *(positions + vertex_index+4) = *(corners + 3);
	    *(positions + vertex_index+5) = *(corners + 1);

	    *(texcoords + vertex_index+0) = vector2_create(x0, z0);
	    *(texcoords + vertex_index+1) = vector2_create(x1, z0);
	    *(texcoords + vertex_index+2) = vector2_create(x0, z1);
	    *(texcoords + vertex_index+3) = vector2_create(x0, z1);
	    *(texcoords + vertex_index+4) = vector2_create(x1, z0);
	    *(texcoords + vertex_index+5) = vector2_create(x1, z1);

	    vertex_index += 6;
	}
    }

    return data;
}

mesh_data
mesh_create_from_heightmap(texture_data heightmap, float size, int resolution, float height_resolution)
{
    mesh_data data = mesh_create_plane_xz(size, resolution);

    size_t height_data_size = (sizeof(float) * (resolution * resolution));
    float* height_data = (float*)malloc(height_data_size);

    int x, y;
    for(y = 0; y < resolution; ++y)
    {
	for(x = 0; x < resolution; ++x)
	{
	    float tx = (float)x / resolution;
	    float ty = (float)y / resolution;

	    vector4 heightmap_color = texture_bilinear_sample(tx, ty, heightmap);
	    *(height_data + (x + y * resolution)) = (heightmap_color.x * height_resolution);
	}
    }
    
    int vertex_index = 0;

    vector3* positions = data.vertices.positions;

    for(y = 0; y < resolution; ++y)
    {
	for(x = 0; x < resolution; ++x)
	{
	    float height0 = height_data[x + (y + 1) * resolution];
	    float height1 = height_data[(x + 1) + (y + 1) * resolution];
	    float height2 = height_data[x + y * resolution];
	    float height3 = height_data[(x + 1) + y * resolution];
	    
	    positions[vertex_index+0].y = height2;
	    positions[vertex_index+1].y = height3;
	    positions[vertex_index+2].y = height0;
	    positions[vertex_index+3].y = height0;
	    positions[vertex_index+4].y = height3;
	    positions[vertex_index+5].y = height1;

	    vertex_index += 6;
	}
    }

    free(height_data);

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
