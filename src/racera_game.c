// racera_game.c

#include "platform.h"
#include "opengl.h"
#include "racera.h"

#include "renderer.h"

static void
set_view(shader_program* shader, shader_uniform_group* uniform_group, fps_camera* camera)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "view");
    if(uniform_info != 0)
    { 
	matrix4 view_matrix = matrix_look_fps(camera->position, camera->pitch, camera->yaw);
	shader_uniform_set_data(uniform_group, uniform_info->location,
				view_matrix.data, sizeof(matrix4));
    }
}


static void
set_view_ui(shader_program* shader, shader_uniform_group* uniform_group)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "view");
    if(uniform_info != 0)
    { 
	vector3 eye = (vector3){{{0.0f, 0.0f, -1.0f}}};
	vector3 at = (vector3){0};
	vector3 up = (vector3){{{0.0f, 1.0f, 0.0f}}};
	
	matrix4 view_matrix = matrix_look_at(eye, at, up);
	shader_uniform_set_data(uniform_group, uniform_info->location,
				view_matrix.data, sizeof(matrix4));
    }
}

static void
set_projection(shader_program* shader, shader_uniform_group* uniform_group,
	       int screen_width, int screen_height)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "projection");
    if(uniform_info != 0)
    {
	float right = (float)screen_width * 0.5f;
	float top = (float)screen_height * 0.5f;
	
	matrix4 projection_matrix = matrix_perspective(80.0f, right / top, 0.01f, 100.0f);
	shader_uniform_set_data(uniform_group, uniform_info->location,
				projection_matrix.data, sizeof(matrix4));
    }
}

static void
set_projection_ui(shader_program* shader, shader_uniform_group* uniform_group,
		  int screen_width, int screen_height)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "projection");
    if(uniform_info != 0)
    {
	matrix4 projection_matrix =
	    matrix_orthographic((float)screen_width, (float)screen_height, 1.0f, 100.0f);
	shader_uniform_set_data(uniform_group, uniform_info->location,
				projection_matrix.data, sizeof(matrix4));
    }
}

static void
set_world(shader_program* shader, shader_uniform_group* uniform_group, vector3 position)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "world");
    if(uniform_info != 0)
    {
	matrix4 world_matrix = matrix_translate(position.x, position.y, position.z);
	
	shader_uniform_set_data(uniform_group, uniform_info->location,
				world_matrix.data, sizeof(matrix4));
    }
}

static void
set_color(shader_program* shader, shader_uniform_group* uniform_group, color color)
{
    shader_uniform* uniform_info = shader_get_uniform(&shader->info, "tint_color");
    if(uniform_info != 0)
    {
	shader_uniform_set_data(uniform_group, uniform_info->location,
				&color, sizeof(color));
    }
}

void
game_update_and_render(game_state* state)
{
    if(!state->loaded_graphics)
    {
	state->gl = load_gl_functions();
	gl_functions* gl = &state->gl;
	
	char vertex_source[1024];
	int vertex_source_length = 1024;
    
	char fragment_source[1024];
	int fragment_source_length = 1024;

	vertex_source_length = platform_read_file("simple.vert", vertex_source, vertex_source_length);
	*(vertex_source + vertex_source_length) = 0;
	fragment_source_length = platform_read_file("simple.frag", fragment_source,
						    fragment_source_length);
	*(fragment_source + fragment_source_length) = 0;

	state->shader = load_shader(gl, vertex_source, vertex_source_length,
				    fragment_source, fragment_source_length);

	state->per_frame_uniforms = shader_uniform_group_create(KB(1));
	state->per_object_uniforms = shader_uniform_group_create(KB(1));

	state->ground =
	    load_mesh(gl, mesh_create_plane_xz(100.0f, 100, (color){1.0f, 1.0f, 1.0f}));
	mesh_data_free(&state->ground.data);
    
	state->cube = load_mesh(gl, mesh_create_cube(1.0f, (color){1.0f, 1.0f, 1.0f}));
	mesh_data_free(&state->cube.data);

	state->pointer = load_mesh(gl, mesh_create_circle(2.0f, 5));
	mesh_data_free(&state->pointer.data);

	state->texture = load_texture(gl, texture_create_checker(128, 128, 64));
	texture_data_free(&state->texture.data);

	glBindTexture(GL_TEXTURE_2D, state->texture.handle);

	state->camera = (fps_camera){0};
	state->camera.position = (vector3){{{0.0f, 1.0f, -2.0f}}};
	
	state->loaded_graphics = 1;
    }
    
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    state->camera.yaw -= (float)state->mouse.relative_x * 0.5f;
    state->camera.pitch -= (float)state->mouse.relative_y * 0.5f;
    if(state->camera.pitch < -90.0f) state->camera.pitch = -90.0f;
    if(state->camera.pitch > 90.0f) state->camera.pitch = 90.0f;

    if(keyboard_is_pressed(&state->keyboard, VKEY_ESCAPE))
    {
	state->should_quit = 1;
    }
	
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

    matrix4 rotate_yaw = matrix_rotation_y(state->camera.yaw);
    matrix4 rotate_pitch = matrix_rotation_x(state->camera.pitch);
    matrix4 camera_rotation = matrix_multiply(rotate_yaw, rotate_pitch);
	
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

    gl_functions* gl = &state->gl;
    gl->glUseProgram(state->shader.program);

    // NOTE: Draw scene
    set_projection(&state->shader, &state->per_frame_uniforms,
		   state->screen_width, state->screen_height);
    set_view(&state->shader, &state->per_frame_uniforms, &state->camera);
    renderer_apply_uniforms(gl, &state->shader, &state->per_frame_uniforms);

    set_color(&state->shader, &state->per_object_uniforms, (color){1.0f, 1.0f, 1.0f});
    set_world(&state->shader, &state->per_object_uniforms, (vector3){{{0.0f, 0.0f, 0.0f}}});
    renderer_apply_uniforms(gl, &state->shader, &state->per_object_uniforms);

    renderer_draw_mesh(gl, &state->ground);
	
    int i;
    for(i = 0; i < state->created_cube_count; i++)
    {
	set_world(&state->shader, &state->per_object_uniforms, *(state->created_cube_positions + i));
	renderer_apply_uniforms(gl, &state->shader, &state->per_object_uniforms);

	renderer_draw_mesh(gl, &state->cube);
    }

    // NOTE: Draw UI
    set_projection_ui(&state->shader, &state->per_frame_uniforms,
		      state->screen_width, state->screen_height);
    set_view_ui(&state->shader, &state->per_frame_uniforms);
    renderer_apply_uniforms(gl, &state->shader, &state->per_frame_uniforms);

    set_color(&state->shader, &state->per_object_uniforms, (color){1.0f, 1.0f, 1.0f});
    set_world(&state->shader, &state->per_object_uniforms, (vector3){{{0.0f, 0.0f, 0.0f}}});
    renderer_apply_uniforms(gl, &state->shader, &state->per_object_uniforms);

    renderer_draw_mesh(gl, &state->pointer);
}
