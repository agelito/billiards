// game.c

#include "platform.h"

#include <stdlib.h>
#include <math.h>

#include "rendering/opengl.h"
#include "game.h"

#include "math.c"
#include "rendering/renderer.c"
#include "font.c"
#include "ui.c"

#include "input/keyboard.c"
#include "input/mouse.c"

static void
game_initialize(game_state* state)
{
    state->gl = load_gl_functions();
    gl_functions* gl = &state->gl;

    state->render_queue = renderer_queue_create(gl, KB(64), KB(16));

    { // NOTE: Load shaders
	platform_log("load shaders\n");
	
	char* vertex_shader = "shaders/simple.vert";
	state->textured =
	    load_shader(gl, vertex_shader, "shaders/textured.frag", 0);
	state->colored =
	    load_shader(gl, vertex_shader, "shaders/colored.frag", 1);
	state->text =
	    load_shader(gl, vertex_shader, "shaders/text.frag", 1);
	state->visualize_normals =
	    load_shader(gl, vertex_shader, "shaders/normals_visualize.frag", 0);
	state->visualize_colors =
	    load_shader(gl, vertex_shader, "shaders/colors_visualize.frag", 0);
	state->visualize_texcoords =
	    load_shader(gl, vertex_shader, "shaders/texcoords_visualize.frag", 0);
	renderer_check_error();
    }

    { // NOTE: Load Meshes
	platform_log("load meshes\n");
	
	state->ground = load_mesh(gl, mesh_create_plane_xz(10.0f, 10), 0);
	mesh_data_free(&state->ground.data);
    
	state->cube = load_mesh(gl, mesh_create_cube(1.0f), 0);
	mesh_data_free(&state->cube.data);

	state->triangle = load_mesh(gl, mesh_create_triangle(1.0f), 0);
	mesh_data_free(&state->triangle.data);

	state->pointer = load_mesh(gl, mesh_create_circle(1.0f, 5), 0);
	mesh_data_free(&state->pointer.data);

	state->cup = load_mesh(gl, obj_load_from_file("cup.obj"), 0);
	mesh_data_free(&state->cup.data);

	state->quad = load_mesh(gl, mesh_create_quad(), 0);
	mesh_data_free(&state->quad.data);

	renderer_check_error();
    }

    { // NOTE: Load Textures
	state->checker = load_texture(gl, texture_create_checker(256, 256, 64));
	texture_data_free(&state->checker.data);
    }

    { // NOTE: Load Fonts
	platform_log("load fonts\n");
	
	state->deja_vu = load_font(gl, font_create_from_file("fonts/DejaVu.fnt"));

	renderer_check_error();
    }

    { // NOTE: Load Materials
	platform_log("load materials\n");

	state->ground_material = material_create(&state->textured, KB(1));
	material_set_texture(&state->ground_material, "main_texture", &state->checker);
	
	state->text_background = material_create(&state->colored, KB(1));
	material_set_color(&state->text_background, "color",
			   vector4_create(0.0f, 0.0f, 0.0f, 0.75f));
    }
    
    state->camera_position = (vector3){{{0.0f, 1.0f, -2.0f}}};
	
    state->initialized = 1;
}

static void
control_camera(game_state* state)
{
    vector3 pitch_yaw_roll = state->camera_pitch_yaw_roll;
    pitch_yaw_roll.y -= (float)state->mouse.relative_x * 0.5f;
    pitch_yaw_roll.x -= (float)state->mouse.relative_y * 0.5f;
    if(pitch_yaw_roll.x < -90.0f) pitch_yaw_roll.x = -90.0f;
    if(pitch_yaw_roll.x > 90.0f) pitch_yaw_roll.x = 90.0f;
    state->camera_pitch_yaw_roll = pitch_yaw_roll;
	
    vector3 camera_movement = (vector3){0};
    if(keyboard_is_down(&state->keyboard, VKEY_W))
    {
	camera_movement.z += 1.0f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_S))
    {
	camera_movement.z -= 1.0f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_A))
    {
	camera_movement.x -= 1.0f;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_D))
    {
	camera_movement.x += 1.0f;
    }

    matrix4 camera_rotation = matrix_rotation_pitch_yaw(pitch_yaw_roll.x, pitch_yaw_roll.y);
    camera_movement = vector3_matrix_multiply(camera_rotation, camera_movement);
    state->camera_position = vector3_add(state->camera_position, camera_movement);

    vector3 camera_forward = (vector3){{{0.0f, 0.0f, 3.0f}}};
    state->camera_forward = vector3_matrix_multiply(camera_rotation, camera_forward);
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

    control_camera(state);

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    { // NOTE: Draw scene
	renderer_queue_push(&state->render_queue, &state->ground,
			    &state->ground_material, matrix_identity());

	float right = (float)state->screen_width * 0.5f;
	float top = (float)state->screen_height * 0.5f;
	matrix4 projection = matrix_perspective(80.0f, right / top, 0.1f, 2000.0f);
	renderer_queue_set_projection(&state->render_queue, projection);

	matrix4 view = matrix_look_fps(state->camera_position, state->camera_pitch_yaw_roll.x,
				       state->camera_pitch_yaw_roll.y);
	renderer_queue_set_view(&state->render_queue, view);

	renderer_queue_process(&state->render_queue);
	renderer_queue_clear(&state->render_queue);
    }

    { // NOTE: Draw UI
	char timings_text[256];
	platform_format(timings_text, 256, "frame time: %f", state->time_frame);

	vector2 text_position = vector2_create((real32)state->screen_width * -0.48f,
						(real32)state->screen_height * 0.45f);
	ui_draw_label(state, text_position, timings_text, 32.0f, &state->deja_vu);

	matrix4 projection = matrix_orthographic((float)state->screen_width,
						 (float)state->screen_height, 1.0f, 100.0f);
	renderer_queue_set_projection(&state->render_queue, projection);

	vector3 eye = (vector3){{{0.0f, 0.0f, -1.0f}}};
	vector3 at = (vector3){0};
	vector3 up = (vector3){{{0.0f, 1.0f, 0.0f}}};
	
	matrix4 view = matrix_look_at(eye, at, up);
	renderer_queue_set_view(&state->render_queue, view);
    
	renderer_queue_process(&state->render_queue);
	renderer_queue_clear(&state->render_queue);
    }
}

