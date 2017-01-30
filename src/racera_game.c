// racera_game.c

#include "platform.h"

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "opengl.h"
#include "racera.h"

#include "math.c"
#include "renderer.c"

#include "keyboard.c"
#include "mouse.c"

static void
game_initialize(game_state* state)
{
    state->gl = load_gl_functions();
    gl_functions* gl = &state->gl;

    state->render_queue = renderer_queue_create(gl, KB(64));

    { // NOTE: Load shaders
	char* vertex_shader = "shaders/simple.vert";
	state->simple_shader       = load_shader(gl, vertex_shader, "shaders/simple.frag");
	state->visualize_normals   = load_shader(gl, vertex_shader, "shaders/normals_visualize.frag");
	state->visualize_colors    = load_shader(gl, vertex_shader, "shaders/colors_visualize.frag");
	state->visualize_texcoords = load_shader(gl, vertex_shader, "shaders/texcoords_visualize.frag");
    }

    state->ground =
	load_mesh(gl, mesh_create_plane_xz(100.0f, 100));
    mesh_data_free(&state->ground.data);
    
    state->cube = load_mesh(gl, mesh_create_cube(1.0f));
    mesh_data_free(&state->cube.data);

    state->triangle = load_mesh(gl, mesh_create_triangle(1.0f));
    mesh_data_free(&state->triangle.data);

    state->pointer = load_mesh(gl, mesh_create_circle(2.0f, 5));
    mesh_data_free(&state->pointer.data);

    state->cup = load_mesh(gl, obj_load_from_file("cup.obj"));
    mesh_data_free(&state->cup.data);
	
    state->checker = load_texture(gl, texture_create_checker(128, 128, 64));
    texture_data_free(&state->checker.data);
	
    state->smiley = load_texture(gl, texture_create_from_tga("smiley_rle.tga"));
    texture_data_free(&state->smiley.data);

    glBindTexture(GL_TEXTURE_2D, state->smiley.handle);

    state->camera_position = (vector3){{{0.0f, 1.0f, -2.0f}}};

    int n;
    for_range(n, 50)
    {
	vector3 position = (vector3){{{platform_randomf(-50.0f, 50.0f),
				       0.0f,
				       platform_randomf(-50.0f, 50.0f)}}};
	    
	*(state->created_cube_positions + state->created_cube_count++) = position;
    }
	
    state->initialized = 1;
}

void
game_update_and_render(game_state* state)
{
    if(!state->initialized)
    {
	game_initialize(state);
    }

    if(keyboard_is_pressed(&state->keyboard, VKEY_ESCAPE))
    {
	state->should_quit = 1;
    }

    state->camera_yaw -= (float)state->mouse.relative_x * 0.5f;
    state->camera_pitch -= (float)state->mouse.relative_y * 0.5f;
    if(state->camera_pitch < -90.0f) state->camera_pitch = -90.0f;
    if(state->camera_pitch > 90.0f) state->camera_pitch = 90.0f;
	
    vector3 camera_movement = (vector3){0};
    if(keyboard_is_down(&state->keyboard, VKEY_W))
    {
	camera_movement.z += 0.1f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_S))
    {
	camera_movement.z -= 0.1f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_A))
    {
	camera_movement.x -= 0.1f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_D))
    {
	camera_movement.x += 0.1f;
    }

    matrix4 camera_rotation = matrix_rotation_pitch_yaw(state->camera_pitch, state->camera_yaw);
    camera_movement = vector3_matrix_multiply(camera_rotation, camera_movement);
    state->camera_position = vector3_add(state->camera_position, camera_movement);

    vector3 camera_forward = (vector3){{{0.0f, 0.0f, 3.0f}}};
    camera_forward = vector3_matrix_multiply(camera_rotation, camera_forward);

    vector3 pointer_location = vector3_add(state->camera_position, camera_forward);
    if(keyboard_is_pressed(&state->keyboard, VKEY_Q) && state->created_cube_count < MAX_CUBES)
    {
	*(state->created_cube_positions + state->created_cube_count++) = pointer_location;
    }

    if(keyboard_is_pressed(&state->keyboard, VKEY_Z) && state->created_cube_count > 0)
    {
	state->created_cube_count -= 1;
    }

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // NOTE: Draw scene
	renderer_queue_push(&state->render_queue, &state->ground,
			    &state->simple_shader, matrix_identity());
	
	int i;
	for(i = 0; i < state->created_cube_count; i++)
	{
	    vector3 position = *(state->created_cube_positions + i);
	    matrix4 transform = matrix_translate(position.x, position.y, position.z);
	
	    renderer_queue_push(&state->render_queue, &state->cup,
				&state->visualize_normals, transform);
	}

	float right = (float)state->screen_width * 0.5f;
	float top = (float)state->screen_height * 0.5f;
	matrix4 projection = matrix_perspective(80.0f, right / top, 0.01f, 100.0f);

	matrix4 view = matrix_look_fps(state->camera_position, state->camera_pitch, state->camera_yaw);

	renderer_queue_process(&state->render_queue, projection, view);
	renderer_queue_clear(&state->render_queue);
    }

    { // NOTE: Draw UI
	matrix4 transform = matrix_identity();
	renderer_queue_push(&state->render_queue, &state->pointer, &state->simple_shader, transform);

	float half_width = (float)state->screen_width * 0.5f;
	float half_height = (float)state->screen_height * 0.5f;
	
	float triangle_size = 128.0f;
	matrix4 triangle_translate = matrix_translate(-half_width + triangle_size * 0.5f,
						      half_height - triangle_size * 0.5f, 0.0f);
	matrix4 triangle_scale = matrix_scale(triangle_size, triangle_size, 1.0f);
	renderer_queue_push(&state->render_queue, &state->triangle, &state->visualize_texcoords,
			    matrix_multiply(triangle_translate, triangle_scale));

	matrix4 projection =
	    matrix_orthographic((float)state->screen_width,
				(float)state->screen_height, 1.0f, 100.0f);

	vector3 eye = (vector3){{{0.0f, 0.0f, -1.0f}}};
	vector3 at = (vector3){0};
	vector3 up = (vector3){{{0.0f, 1.0f, 0.0f}}};
	
	matrix4 view = matrix_look_at(eye, at, up);
    
	renderer_queue_process(&state->render_queue, projection, view);
	renderer_queue_clear(&state->render_queue);
    }
}

