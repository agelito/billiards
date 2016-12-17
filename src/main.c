// main.c

#include <stdio.h>
#include <stdlib.h>

#include "aren.h"
#include "platform.h"
#include "gl_extensions.h"
#include "window.h"
#include "keyboard_x11.h"

#include "mesh.h"

typedef struct
{
    GLuint vertex;
    GLuint fragment;
    GLuint program;
} shader_program;

shader_program load_shader(gl_functions* gl, char* vertex_source, int vertex_source_length, char* fragment_source, int fragment_source_length)
{
    shader_program shader;

    shader.vertex = gl->glCreateShader(GL_VERTEX_SHADER);
    shader.fragment = gl->glCreateShader(GL_FRAGMENT_SHADER);

    gl->glShaderSource(shader.vertex, 1, (const GLchar**)&vertex_source, &vertex_source_length);
    gl->glShaderSource(shader.fragment, 1, (const GLchar**)&fragment_source, &fragment_source_length);

    int successful_compile = 0;
    
    gl->glCompileShader(shader.vertex);
    gl->glGetShaderiv(shader.vertex, GL_COMPILE_STATUS, &successful_compile);
    if(!successful_compile)
    {
	printf("failed to compile vertex shader:\n %s\n", vertex_source);
    }

    gl->glCompileShader(shader.fragment);
    gl->glGetShaderiv(shader.fragment, GL_COMPILE_STATUS, &successful_compile);
    if(!successful_compile)
    {
	printf("failed to compile fragment shader:\n %s\n", fragment_source);
    }

    shader.program = gl->glCreateProgram();
    gl->glAttachShader(shader.program, shader.vertex);
    gl->glAttachShader(shader.program, shader.fragment);
    gl->glLinkProgram(shader.program);

    int successful_link = 0;
    gl->glGetProgramiv(shader.program, GL_LINK_STATUS, &successful_link);
    if(!successful_link)
    {
	GLsizei info_log_length;
	char info_log[1024];
	gl->glGetProgramInfoLog(shader.program, 1024, &info_log_length, (GLchar*)info_log);
	printf("failed to link shader program:\n%s\n\n", info_log);
	printf("sources:\n%s\n\n%s\n\n", vertex_source, fragment_source);
    }

    return shader;
}

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
	printf("event type: %d\n", event.type);

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
    
    FILE* file = fopen("simple.vert", "rb");
    if(!file)
    {
	printf("can't open file simple.vert\n");
	exit(0);
    }

    vertex_source_length = read_file("simple.vert", vertex_source, vertex_source_length);
    fragment_source_length = read_file("simple.frag", fragment_source, fragment_source_length);

    shader_program shader = load_shader(&gl, vertex_source, vertex_source_length, fragment_source, fragment_source_length);

    mesh_data triangle_data = mesh_create_triangle(1.0f);
    loaded_mesh triangle = load_mesh(&gl, triangle_data);
    mesh_data_free(&triangle_data);
    
    while(handle_window_events(&window_context, &keyboard))
    {
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	gl.glUseProgram(shader.program);
	gl.glBindVertexArray(triangle.vao);
	gl.glBindBuffer(GL_ARRAY_BUFFER, triangle.vbo);
	glDrawArrays(GL_TRIANGLES, 0, triangle.data.vertex_count);

	redraw_window(&window_context);
	
	platform_sleep(1);
    }

    XCloseDisplay(window_context.display);

    return 0;
}
