// racera_game.c

#include "platform.h"
#include "opengl.h"
#include "racera.h"

#include "renderer.h"

static matrix4
camera_rotation_matrix(fps_camera* camera)
{
    matrix4 rotate_yaw = matrix_rotation_y(camera->yaw);
    matrix4 rotate_pitch = matrix_rotation_x(camera->pitch);
    matrix4 camera_rotation = matrix_multiply(rotate_yaw, rotate_pitch);
    return camera_rotation;
}

#include <stdio.h>

void
game_update_and_render(game_state* state)
{
    if(!state->loaded_graphics)
    {
	state->gl = load_gl_functions();
	gl_functions* gl = &state->gl;

	state->render_queue = renderer_queue_create(gl, KB(64));
	
	read_file vertex_source = platform_read_file("simple.vert", 1);
	read_file fragment_source = platform_read_file("simple.frag", 1);

	state->simple_shader = load_shader(gl, vertex_source.data, vertex_source.size,
				    fragment_source.data, fragment_source.size);


	platform_free_file(&fragment_source);

	fragment_source = platform_read_file("uv_visualize.frag", 1);
	state->uv_shader = load_shader(gl, vertex_source.data, vertex_source.size,
				       fragment_source.data, fragment_source.size);

	platform_free_file(&fragment_source);
	platform_free_file(&vertex_source);				       

	state->ground =
	    load_mesh(gl, mesh_create_plane_xz(100.0f, 100, (color){1.0f, 1.0f, 1.0f}));
	mesh_data_free(&state->ground.data);
    
	state->cube = load_mesh(gl, mesh_create_cube(1.0f, (color){1.0f, 1.0f, 1.0f}));
	mesh_data_free(&state->cube.data);

	state->pointer = load_mesh(gl, mesh_create_circle(2.0f, 5));
	mesh_data_free(&state->pointer.data);
	
	state->checker = load_texture(gl, texture_create_checker(128, 128, 64));
	texture_data_free(&state->checker.data);
	
	state->smiley = load_texture(gl, texture_create_from_tga("smiley_rle.tga"));
	texture_data_free(&state->smiley.data);

	glBindTexture(GL_TEXTURE_2D, state->smiley.handle);

	state->camera = (fps_camera){0};
	state->camera.position = (vector3){{{0.0f, 1.0f, -2.0f}}};
	
	state->loaded_graphics = 1;
    }

    if(keyboard_is_pressed(&state->keyboard, VKEY_ESCAPE))
    {
	state->should_quit = 1;
    }

    state->camera.yaw -= (float)state->mouse.relative_x * 0.5f;
    state->camera.pitch -= (float)state->mouse.relative_y * 0.5f;
    if(state->camera.pitch < -90.0f) state->camera.pitch = -90.0f;
    if(state->camera.pitch > 90.0f) state->camera.pitch = 90.0f;
	
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

    matrix4 camera_rotation = camera_rotation_matrix(&state->camera);
    camera_movement = vector3_matrix_multiply(camera_rotation, camera_movement);
    state->camera.position = vector3_add(state->camera.position, camera_movement);

    vector3 camera_forward = (vector3){{{0.0f, 0.0f, 3.0f}}};
    camera_forward = vector3_matrix_multiply(camera_rotation, camera_forward);

    vector3 pointer_location = vector3_add(state->camera.position, camera_forward);
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
	
	    renderer_queue_push(&state->render_queue, &state->cube, &state->simple_shader, transform);
	}

	float right = (float)state->screen_width * 0.5f;
	float top = (float)state->screen_height * 0.5f;
	matrix4 projection = matrix_perspective(80.0f, right / top, 0.01f, 100.0f);

	fps_camera* camera = &state->camera;
	matrix4 view = matrix_look_fps(camera->position, camera->pitch, camera->yaw);

	renderer_queue_process(&state->render_queue, projection, view);
	renderer_queue_clear(&state->render_queue);
    }

    { // NOTE: Draw UI
	matrix4 transform = matrix_identity();
	renderer_queue_push(&state->render_queue, &state->pointer, &state->simple_shader, transform);

	matrix4 projection =
	    matrix_orthographic((float)state->screen_width, (float)state->screen_height, 1.0f, 100.0f);

	vector3 eye = (vector3){{{0.0f, 0.0f, -1.0f}}};
	vector3 at = (vector3){0};
	vector3 up = (vector3){{{0.0f, 1.0f, 0.0f}}};
	
	matrix4 view = matrix_look_at(eye, at, up);
    
	renderer_queue_process(&state->render_queue, projection, view);
	renderer_queue_clear(&state->render_queue);
    }
}

