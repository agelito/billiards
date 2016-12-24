// main.c

#include <stdio.h>
#include <stdlib.h>

#include "aren.h"
#include "platform.h"
#include "gl_extensions.h"

#include "window_x11.h"
#include "keyboard_x11.h"

#include "math.h"
#include "shader.h"
#include "mesh.h"

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

int handle_window_events(window_and_gl_context* window_context, keycode_map* keyboard)
{
    int window_is_open = 1;

    while(XPending(window_context->display))
    {
	XEvent event;
	XNextEvent(window_context->display, &event);

        if(event.type == KeyPress)
	{
	    if(keycode_is_symbol(keyboard, event.xkey.keycode, XK_Escape))
	    {
		destroy_window(window_context);
	    }
	}
	else if(event.type == ConfigureNotify)
	{
	    resize_viewport(window_context);
	}
	else if(event.type == DestroyNotify)
	{
	    destroy_gl_context(window_context);
	    window_is_open = 0;
	}
    }

    return window_is_open;
}

void set_shader_uniforms(gl_functions* gl, GLuint program, int screen_width, int screen_height)
{
    GLint projection_matrix_location = gl->glGetUniformLocation(program, "projection");
    if(projection_matrix_location != -1)
    {
	float right = (float)screen_width * 0.5f;
	float top = (float)screen_height * 0.5f;
	
	matrix4 projection_matrix = matrix_perspective(60.0f, right / top, 0.01f, 100.0f);
	// matrix4 projection_matrix = matrix_orthographic(2.0f, top / right * 2.0f, 0.01f, 100.0f);
	//matrix4 projection_matrix = matrix_identity();
	gl->glUniformMatrix4fv(projection_matrix_location, 1, GL_FALSE, projection_matrix.data);
    }

    GLint view_matrix_location = gl->glGetUniformLocation(program, "view");
    if(view_matrix_location != -1)
    {
	vector3 eye = (vector3){{{0.0f, 0.0f, 5.0f}}};

	static float pitch = 0.0f;
	static float yaw = 0.0f;

	matrix4 view_matrix = matrix_look_fps(eye, pitch, yaw);
	gl->glUniformMatrix4fv(view_matrix_location, 1, GL_FALSE, view_matrix.data);
    }

    GLint world_matrix_location = gl->glGetUniformLocation(program, "world");
    if(world_matrix_location != -1)
    {
	static float rotation = 0.0f;
	rotation = rotation + 0.009f;

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

    keycode_map keyboard = create_keycode_map(window_context.display);
    
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

    glCullFace(GL_BACK);
    
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    
    while(handle_window_events(&window_context, &keyboard))
    {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl.glUseProgram(shader.program);
	set_shader_uniforms(&gl, shader.program, window_context.width, window_context.height);
	
	gl.glBindVertexArray(mesh.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
	glDrawArrays(GL_TRIANGLES, 0, mesh.data.vertex_count);

	redraw_window(&window_context);
	
	platform_sleep(1);
    }

    XCloseDisplay(window_context.display);

    return 0;
}
