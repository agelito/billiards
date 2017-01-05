// main.c

#include "racera.h"
#include "platform.h"
#include "opengl.h"

#include "window_x11.h"
#include "keyboard_x11.h"
#include "mouse_x11.h"

#include "math.h"
#include "shader.h"
#include "mesh.h"
#include "texture.h"

typedef struct
{
    vector3 position;
    float yaw;
    float pitch;
} fps_camera;

void set_view(gl_functions* gl, GLuint program, fps_camera* camera)
{
    GLint view_matrix_location = gl->glGetUniformLocation(program, "view");
    if(view_matrix_location != -1)
    {
	matrix4 view_matrix = matrix_look_fps(camera->position, camera->pitch, camera->yaw);
	gl->glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.data);
    }
}


void set_view_ui(gl_functions* gl, GLuint program)
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

void set_projection(gl_functions* gl, GLuint program, int screen_width, int screen_height)
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

void set_projection_ui(gl_functions* gl, GLuint program, int screen_width, int screen_height)
{
    GLint projection_matrix_location = gl->glGetUniformLocation(program, "projection");
    if(projection_matrix_location != -1)
    {
	matrix4 projection_matrix = matrix_orthographic((float)screen_width, (float)screen_height, 1.0f, 100.0f);
	gl->glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix.data);
    }
}

void set_world(gl_functions* gl, GLuint program, vector3 position)
{
    GLint world_matrix_location = gl->glGetUniformLocation(program, "world");
    if(world_matrix_location != -1)
    {
	matrix4 world_matrix = matrix_translate(position.x, position.y, position.z);
	gl->glUniformMatrix4fv(world_matrix_location, 1, GL_FALSE, world_matrix.data);
    }
}

void set_color(gl_functions* gl, GLuint program, color color)
{
    GLint color_location = gl->glGetUniformLocation(program, "tint_color");
    if(color_location != -1)
    {
	gl->glUniform3fv(color_location, 1, &color.r);
    }
}

int main(int argc, char* argv[])
{
    UNUSED(argc);
    UNUSED(argv);

    char exe_dir[1024];
    int exe_dir_length = 1024;
    
    exe_dir_length = platform_executable_directory(exe_dir, exe_dir_length);
    platform_set_working_directory(exe_dir);
    
    window_x11 window_context = create_window(1280, 720, "racera");

    keyboard_input keyboard = keyboard_init(window_context.display);
    mouse_input mouse = init_mouse_input(window_context.display, window_context.window);
    
    gl_functions gl = load_gl_functions();

    char vertex_source[1024];
    int vertex_source_length = 1024;
    
    char fragment_source[1024];
    int fragment_source_length = 1024;

    vertex_source_length = platform_read_file("simple.vert", vertex_source, vertex_source_length);
    *(vertex_source + vertex_source_length) = 0;
    fragment_source_length = platform_read_file("simple.frag", fragment_source, fragment_source_length);
    *(fragment_source + fragment_source_length) = 0;

    shader_program shader = load_shader(&gl, vertex_source, vertex_source_length, fragment_source, fragment_source_length);

    loaded_mesh ground = load_mesh(&gl, mesh_create_plane_xz(100.0f, 100, (color){1.0f, 1.0f, 1.0f}));
    mesh_data_free(&ground.data);
    
    loaded_mesh mesh = load_mesh(&gl, mesh_create_cube(1.0f, (color){1.0f, 1.0f, 1.0f}));
    mesh_data_free(&mesh.data);

    loaded_mesh pointer = load_mesh(&gl, mesh_create_circle(2.0f, 5));
    mesh_data_free(&mesh.data);

    loaded_texture texture = load_texture(&gl, texture_create_checker(128, 128, 64));
    texture_data_free(&texture.data);

#if 1
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
#else
    glPolygonMode(GL_BACK, GL_LINE);
#endif
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    fps_camera camera = (fps_camera){0};
    camera.position = (vector3){{{0.0f, 1.0f, -2.0f}}};

    int created_cube_count = 0;

    #define MAX_CUBES 2048
    vector3 created_cube_positions[MAX_CUBES];
    
    glBindTexture(GL_TEXTURE_2D, texture.handle);

    while(handle_window_events(&window_context, &keyboard, &mouse))
    {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	camera.yaw -= (float)mouse.movement_delta_x * 0.5f;
	camera.pitch -= (float)mouse.movement_delta_y * 0.5f;
	if(camera.pitch < -90.0f) camera.pitch = -90.0f;
	if(camera.pitch > 90.0f) camera.pitch = 90.0f;
	
	if(is_pressed(&keyboard, VKEY_ESCAPE))
	{
	    destroy_window(&window_context);
        break;
	}
	
	vector3 camera_movement = (vector3){0};
	if(is_down(&keyboard, VKEY_W))
	{
	    camera_movement.z += 0.1f;
	}

	if(is_down(&keyboard, VKEY_S))
	{
	    camera_movement.z -= 0.1f;
	}

	if(is_down(&keyboard, VKEY_A))
	{
	    camera_movement.x -= 0.1f;
	}

	if(is_down(&keyboard, VKEY_D))
	{
	    camera_movement.x += 0.1f;
	}

	matrix4 rotate_yaw = matrix_rotation_y(camera.yaw);
	matrix4 rotate_pitch = matrix_rotation_x(camera.pitch);
	matrix4 camera_rotation = matrix_multiply(rotate_yaw, rotate_pitch);
	
	camera_movement = vector3_matrix_multiply(camera_rotation, camera_movement);
	camera.position = vector3_add(camera.position, camera_movement);

	vector3 camera_forward = (vector3){{{0.0f, 0.0f, 3.0f}}};
	camera_forward = vector3_matrix_multiply(camera_rotation, camera_forward);

	vector3 pointer_location = vector3_add(camera.position, camera_forward);

	if(is_pressed(&keyboard, VKEY_Q) && created_cube_count < MAX_CUBES)
	{
	    *(created_cube_positions + created_cube_count++) = pointer_location;
	}

	if(is_pressed(&keyboard, VKEY_Z) && created_cube_count > 0)
	{
	    created_cube_count -= 1;
	}
	
	gl.glUseProgram(shader.program);
	set_projection(&gl, shader.program, window_context.width, window_context.height);
	set_view(&gl, shader.program, &camera);

	set_color(&gl, shader.program, (color){0.87f, 0.82f, 0.86f});

	gl.glBindVertexArray(ground.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, ground.vbo);

	set_world(&gl, shader.program, (vector3){{{0.0f, 0.0f, 0.0f}}});
	glDrawArrays(GL_TRIANGLES, 0, ground.data.vertex_count);
	
	gl.glBindVertexArray(mesh.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	
	int i;
	for(i = 0; i < created_cube_count; i++)
	{
	    set_world(&gl, shader.program, *(created_cube_positions + i));
	    glDrawArrays(GL_TRIANGLES, 0, mesh.data.vertex_count);
	}

	set_color(&gl, shader.program, (color){1.0f, 1.0f, 1.0f});

	// NOTE: Draw UI
	set_projection_ui(&gl, shader.program, window_context.width, window_context.height);
	set_view_ui(&gl, shader.program);

	set_world(&gl, shader.program, (vector3){{{0.0f, 0.0f, 0.0f}}});
	
	gl.glBindVertexArray(pointer.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, pointer.vbo);
	glDrawArrays(GL_TRIANGLES, 0, pointer.data.vertex_count);

	redraw_window(&window_context);

	mouse.movement_delta_x = 0;
	mouse.movement_delta_y = 0;

	keyboard_reset_state(&keyboard);
	
	platform_sleep(1);
    }

    return 0;
}
