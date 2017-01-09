// racera_game.c

#include "platform.h"
#include "opengl.h"
#include "racera.h"

static void
set_view(gl_functions* gl, GLuint program, fps_camera* camera)
{
    GLint view_matrix_location = gl->glGetUniformLocation(program, "view");
    if(view_matrix_location != -1)
    {
	matrix4 view_matrix = matrix_look_fps(camera->position, camera->pitch, camera->yaw);
	gl->glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.data);
    }
}


static void
set_view_ui(gl_functions* gl, GLuint program)
{
    GLint view_matrix_location = gl->glGetUniformLocation(program, "view");
    if(view_matrix_location != -1)
    {
	vector3 eye = (vector3){{{0.0f, 0.0f, -1.0f}}};
	vector3 at = (vector3){0};
	vector3 up = (vector3){{{0.0f, 1.0f, 0.0f}}};
	
	matrix4 view_matrix = matrix_look_at(eye, at, up);
	gl->glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.data);
    }
}

static void
set_projection(gl_functions* gl, GLuint program, int screen_width, int screen_height)
{
    GLint projection_matrix_location = gl->glGetUniformLocation(program, "projection");
    if(projection_matrix_location != -1)
    {
	float right = (float)screen_width * 0.5f;
	float top = (float)screen_height * 0.5f;
	
	matrix4 projection_matrix = matrix_perspective(80.0f, right / top, 0.01f, 100.0f);
	gl->glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix.data);
    }
}

static void
set_projection_ui(gl_functions* gl, GLuint program, int screen_width, int screen_height)
{
    GLint projection_matrix_location = gl->glGetUniformLocation(program, "projection");
    if(projection_matrix_location != -1)
    {
	matrix4 projection_matrix = matrix_orthographic((float)screen_width, (float)screen_height, 1.0f, 100.0f);
	gl->glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix.data);
    }
}

static void
set_world(gl_functions* gl, GLuint program, vector3 position)
{
    GLint world_matrix_location = gl->glGetUniformLocation(program, "world");
    if(world_matrix_location != -1)
    {
	matrix4 world_matrix = matrix_translate(position.x, position.y, position.z);
	gl->glUniformMatrix4fv(world_matrix_location, 1, GL_FALSE, world_matrix.data);
    }
}

static void
set_color(gl_functions* gl, GLuint program, color color)
{
    GLint color_location = gl->glGetUniformLocation(program, "tint_color");
    if(color_location != -1)
    {
	gl->glUniform3fv(color_location, 1, &color.r);
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

	shader_reflect(gl, &state->shader);

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
    set_projection(gl, state->shader.program, state->screen_width, state->screen_height);
    set_view(gl, state->shader.program, &state->camera);

    set_color(gl, state->shader.program, (color){0.87f, 0.82f, 0.86f});

    gl->glBindVertexArray(state->ground.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, state->ground.vbo);

    set_world(gl, state->shader.program, (vector3){{{0.0f, 0.0f, 0.0f}}});
    glDrawArrays(GL_TRIANGLES, 0, state->ground.data.vertex_count);
	
    gl->glBindVertexArray(state->cube.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, state->cube.vbo);
	
    int i;
    for(i = 0; i < state->created_cube_count; i++)
    {
	set_world(gl, state->shader.program, *(state->created_cube_positions + i));
	glDrawArrays(GL_TRIANGLES, 0, state->cube.data.vertex_count);
    }

    set_color(gl, state->shader.program, (color){1.0f, 1.0f, 1.0f});

    // NOTE: Draw UI
    set_projection_ui(gl, state->shader.program, state->screen_width, state->screen_height);
    set_view_ui(gl, state->shader.program);

    set_world(gl, state->shader.program, (vector3){{{0.0f, 0.0f, 0.0f}}});
	
    gl->glBindVertexArray(state->pointer.vao);
    gl->glBindBuffer(GL_ARRAY_BUFFER, state->pointer.vbo);
    glDrawArrays(GL_TRIANGLES, 0, state->pointer.data.vertex_count);
}
