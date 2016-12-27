// main.c

#include <stdio.h>
#include <stdlib.h>

#include "aren.h"
#include "platform.h"
#include "gl_extensions.h"

#include "window_x11.h"
#include "keyboard_x11.h"
#include "mouse_x11.h"

#include "math.h"
#include "shader.h"
#include "mesh.h"

typedef struct
{
    vector3 position;
    float yaw;
    float pitch;
} fps_camera;

int read_file(char* path, char* destination, int destination_size)
{
    FILE* file = fopen(path, "rb");
    if(!file)
    {
	printf("can't open file %s\n", path);
	return 0;
    }

    fseek(file, 0, SEEK_END);
    int size = ftell(file);

    if(size >= destination_size)
    {
	size = (destination_size - 1);
    }
    
    fseek(file, 0, SEEK_SET);
    fread(destination, 1, size, file);
    fclose(file);

    destination[size] = 0;
    return size;
}

void set_camera_uniforms(gl_functions* gl, GLuint program, fps_camera* camera)
{
    GLint view_matrix_location = gl->glGetUniformLocation(program, "view");
    if(view_matrix_location != -1)
    {
	matrix4 view_matrix = matrix_look_fps(camera->position, camera->pitch, camera->yaw);
	gl->glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.data);
    }
}

void set_shader_uniforms(gl_functions* gl, GLuint program, int screen_width, int screen_height)
{
    GLint projection_matrix_location = gl->glGetUniformLocation(program, "projection");
    if(projection_matrix_location != -1)
    {
	float right = (float)screen_width * 0.5f;
	float top = (float)screen_height * 0.5f;
	
	matrix4 projection_matrix = matrix_perspective(80.0f, right / top, 0.01f, 100.0f);
	gl->glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix.data);
    }

    GLint world_matrix_location = gl->glGetUniformLocation(program, "world");
    if(world_matrix_location != -1)
    {
	static float rotation = 0.0f;
	// rotation = rotation + 0.04f;

	matrix4 world_matrix = matrix_rotation_y(rotation);
	
	gl->glUniformMatrix4fv(world_matrix_location, 1, GL_FALSE, world_matrix.data);
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
    
    window_and_gl_context window_context = create_window_and_gl_context(500, 500, "aren");

    keyboard_input keyboard = keyboard_init(window_context.display);
    mouse_input mouse = init_mouse_input(window_context.display, window_context.window);
    
    gl_functions gl = load_gl_functions();

    char vertex_source[1024];
    int vertex_source_length = 0;
    
    char fragment_source[1024];
    int fragment_source_length = 0;

    vertex_source_length = read_file("simple.vert", vertex_source, vertex_source_length);
    fragment_source_length = read_file("simple.frag", fragment_source, fragment_source_length);

    shader_program shader = load_shader(&gl, vertex_source, vertex_source_length, fragment_source, fragment_source_length);

    //loaded_mesh mesh = load_mesh(&gl, mesh_create_circle(300.0f, 9));
    loaded_mesh mesh = load_mesh(&gl, mesh_create_cube(1.0f));
    mesh_data_free(&mesh.data);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);
    glPolygonMode(GL_BACK, GL_LINE);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

    fps_camera camera = (fps_camera){0};
    camera.position = (vector3){{{0.0f, 0.0f, -2.0f}}};
    
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
	
	gl.glUseProgram(shader.program);
	set_shader_uniforms(&gl, shader.program, window_context.width, window_context.height);
	set_camera_uniforms(&gl, shader.program, &camera);
	
	gl.glBindVertexArray(mesh.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glDrawArrays(GL_TRIANGLES, 0, mesh.data.vertex_count);

	redraw_window(&window_context);

	mouse.movement_delta_x = 0;
	mouse.movement_delta_y = 0;

	keyboard_reset_state(&keyboard);
	
	platform_sleep(1);
    }

    XCloseDisplay(window_context.display);

    return 0;
}
