// main.c

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "window.h"

typedef struct
{
    GLint vao;
    GLint vbo;
} mesh_buffer;

mesh_buffer load_triangle()
{
    mesh_buffer mesh;
    
    GLint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    mesh.vao = vao;

    GLint vbo;
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    mesh.vbo = vbo;

    GLfloat vertices[] = {
	0.0f, 0.0f, 0.0f,
	-0.75f, -0.75f, 0.0f,
	0.75f, -0.75f, 0.0
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // TODO: Vertex attribute bind point hardcoded.
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    return mesh;
}

int main(int argc, char* argv[])
{
    window_and_gl_context window_context = create_window_and_gl_context(500, 500, "aren");

    mesh_buffer triangle_buffer = load_triangle();
    
    while(1)
    {
	glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	XEvent event;
	XNextEvent(window_context.display, &event);

	if(event.type == Expose)
	{
	    redraw_window(&window_context);
	}
	else if(event.type == KeyPress)
	{
	    break;
	}
    }

    destroy_window_and_gl_context(&window_context);

    return 0;
}
