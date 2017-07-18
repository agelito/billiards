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

    state->render_queue = renderer_queue_create(gl, KB(4), KB(16));

    float ambient_coefficient = 0.01f;
    shader_uniform_set_data(&state->render_queue.uniforms, hash_string("ambient_coefficient"),
			    &ambient_coefficient, sizeof(float));

    vector4 light_color_intensity = vector4_create(1.0, 1.0, 1.0, 1.0);
    shader_uniform_set_data(&state->render_queue.uniforms, hash_string("light_color_intensity"),
			    &light_color_intensity, sizeof(vector4));

    vector3 light_direction = vector3_create(0.0f, 1.0f, -1.0f);
    shader_uniform_set_data(&state->render_queue.uniforms, hash_string("light_direction"),
			    &light_direction, sizeof(vector3));

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
	state->visualize_tangents =
	    load_shader(gl, vertex_shader, "shaders/tangents_visualize.frag", 0);
	state->lightning =
	    load_shader(gl, vertex_shader, "shaders/lightning.frag", 0);
	renderer_check_error();
    }

    { // NOTE: Load Meshes
	platform_log("load meshes\n");

	mesh_data ground_mesh = mesh_create_plane_xz(60.0f, 20);
	mesh_generate_tangents(&ground_mesh);
	
	state->ground = load_mesh(gl, ground_mesh, 0);
	mesh_data_free(&ground_mesh);
    
	state->quad = load_mesh(gl, mesh_create_quad(), 0);
	mesh_data_free(&state->quad.data);

	mesh_data sphere_mesh = mesh_create_sphere(1.0f, 5);
	//mesh_data sphere_mesh = obj_load_from_file("sphere.obj");
	mesh_generate_tangents(&sphere_mesh);
	
	state->sphere = load_mesh(gl, sphere_mesh, TEXTURE_REPEAT);
	mesh_data_free(&sphere_mesh);

	renderer_check_error();
    }

    { // NOTE: Load Textures
	state->checker = load_texture(gl, texture_create_checker(256, 256, 64), 0);
	texture_data_free(&state->checker.data);

	state->checker2 = load_texture(gl, texture_create_checker(256, 256, 16), 0);
	texture_data_free(&state->checker.data);

	state->billiard_balls[0] = load_texture(gl, texture_create_from_tga("billiard_ball_1.tga"), 1);
	texture_data_free(&state->billiard_balls[0].data);

	state->billiard_balls[1] = load_texture(gl, texture_create_from_tga("billiard_ball_2.tga"), 1);
	texture_data_free(&state->billiard_balls[1].data);

	state->billiard_balls[2] = load_texture(gl, texture_create_from_tga("billiard_ball_3.tga"), 1);
	texture_data_free(&state->billiard_balls[2].data);

	state->billiard_balls[3] = load_texture(gl, texture_create_from_tga("billiard_ball_4.tga"), 1);
	texture_data_free(&state->billiard_balls[3].data);

	state->billiard_balls[4] = load_texture(gl, texture_create_from_tga("billiard_ball_5.tga"), 1);
	texture_data_free(&state->billiard_balls[4].data);

	state->billiard_balls[5] = load_texture(gl, texture_create_from_tga("billiard_ball_6.tga"), 1);
	texture_data_free(&state->billiard_balls[5].data);

	state->billiard_balls[6] = load_texture(gl, texture_create_from_tga("billiard_ball_7.tga"), 1);
	texture_data_free(&state->billiard_balls[6].data);

	state->billiard_balls[7] = load_texture(gl, texture_create_from_tga("billiard_ball_8.tga"), 1);
	texture_data_free(&state->billiard_balls[7].data);

	state->billiard_balls[8] = load_texture(gl, texture_create_from_tga("billiard_ball_9.tga"), 1);
	texture_data_free(&state->billiard_balls[8].data);

	state->billiard_balls[9] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_10.tga"), 1);
	texture_data_free(&state->billiard_balls[9].data);

	state->billiard_balls[10] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_11.tga"), 1);
	texture_data_free(&state->billiard_balls[10].data);

	state->billiard_balls[11] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_12.tga"), 1);
	texture_data_free(&state->billiard_balls[11].data);

	state->billiard_balls[12] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_13.tga"), 1);
	texture_data_free(&state->billiard_balls[12].data);

	state->billiard_balls[13] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_14.tga"), 1);
	texture_data_free(&state->billiard_balls[13].data);

	state->billiard_balls[14] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_15.tga"), 1);
	texture_data_free(&state->billiard_balls[14].data);

	state->billiard_balls[15] =
	    load_texture(gl, texture_create_from_tga("billiard_ball_16.tga"), 1);
	texture_data_free(&state->billiard_balls[15].data);

	state->table_cloth =
	    load_texture(gl, texture_create_from_tga("pool_table_cloth.tga"), TEXTURE_MIRROR);
	texture_data_free(&state->table_cloth.data);
    }

    { // NOTE: Load Fonts
	platform_log("load fonts\n");
	
	state->deja_vu = load_font(gl, font_create_from_file("fonts/DejaVu.fnt"));

	renderer_check_error();
    }

    { // NOTE: Load Materials
	platform_log("load materials\n");

	state->ground_material = material_create(&state->lightning, 68);
	material_set_texture(&state->ground_material, "main_texture", &state->checker);
	material_set_scalar(&state->ground_material, "roughness", 1.0f);

	state->sphere_material = material_create(&state->lightning, 68);
	material_set_texture(&state->sphere_material, "main_texture", &state->checker2);
	material_set_scalar(&state->sphere_material, "roughness", 0.0f);

	material billiard_ball_material = material_create(&state->lightning, 68);
	material_set_scalar(&billiard_ball_material, "roughness", 0.5f);

	int i;
	for_range(i, 16)
	{
	    state->billiard[i] = material_copy(&billiard_ball_material);
	    material_set_texture(&state->billiard[i], "main_texture", &state->billiard_balls[i]);
	}

	state->pool_table = material_create(&state->textured, 68);
	material_set_texture(&state->pool_table, "main_texture", &state->table_cloth);

	state->text_background = material_create(&state->colored, 32);
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

    const float camera_movement_speed = 0.1f;
    
    vector3 camera_movement = (vector3){0};
    if(keyboard_is_down(&state->keyboard, VKEY_W))
    {
	camera_movement.z += camera_movement_speed;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_S))
    {
	camera_movement.z -= camera_movement_speed;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_A))
    {
	camera_movement.x -= camera_movement_speed;
    }

    if(keyboard_is_down(&state->keyboard, VKEY_D))
    {
	camera_movement.x += camera_movement_speed;
    }

    matrix4 camera_rotation = matrix_rotation_pitch_yaw(pitch_yaw_roll.x, pitch_yaw_roll.y);
    camera_movement = vector3_matrix_multiply(camera_rotation, camera_movement);
    state->camera_position = vector3_add(state->camera_position, camera_movement);

    vector3 camera_forward = (vector3){{{0.0f, 0.0f, 3.0f}}};
    state->camera_forward = vector3_matrix_multiply(camera_rotation, camera_forward);

    shader_uniform_set_data(&state->render_queue.uniforms, hash_string("camera_position"),
			    state->camera_position.data, sizeof(vector3));
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

    static real32 rotation_y = 90.0f;
    static real32 rotation_x = 0.0f;
    rotation_x += 0.5f;
    rotation_y += 3.0f;

    matrix4 ball_rotation = matrix_multiply(matrix_rotation_x(rotation_x),
					    matrix_rotation_y(rotation_y));

    renderer_queue_push_clear(&state->render_queue, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT,
			      (float[4]){0.1f, 0.1f, 0.1f, 1.0f});

    { // NOTE: Draw scene
	renderer_queue_push_draw(&state->render_queue, &state->ground,
				 &state->pool_table, matrix_translate(0.0f, 0.0f, 0.0f));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->sphere_material,
				 matrix_multiply(matrix_translate(-24.0f, 1.0f, 0.0f), ball_rotation));

	// NOTE: Draw billiard balls
	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[0],
				 matrix_multiply(matrix_translate(-21.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[1],
				 matrix_multiply(matrix_translate(-18.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[2],
				 matrix_multiply(matrix_translate(-15.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[3],
				 matrix_multiply(matrix_translate(-12.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[4],
				 matrix_multiply(matrix_translate(-9.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[5],
				 matrix_multiply(matrix_translate(-6.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[6],
				 matrix_multiply(matrix_translate(-3.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[7],
				 matrix_multiply(matrix_translate(0.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[8],
				 matrix_multiply(matrix_translate(3.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[9],
				 matrix_multiply(matrix_translate(6.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[10],
				 matrix_multiply(matrix_translate(9.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[11],
				 matrix_multiply(matrix_translate(12.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[12],
				 matrix_multiply(matrix_translate(15.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[13],
				 matrix_multiply(matrix_translate(18.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[14],
				 matrix_multiply(matrix_translate(21.0f, 1.0f, 0.0f), ball_rotation));

	renderer_queue_push_draw(&state->render_queue, &state->sphere, &state->billiard[15],
				 matrix_multiply(matrix_translate(24.0f, 1.0f, 0.0f), ball_rotation));
	

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

