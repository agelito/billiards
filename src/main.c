// main.c

#include <stdio.h>
#include <stdlib.h>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "window.h"

#define GL_PROC_DEC(function, ...) typedef void (APIENTRYP GL_PROC_##function) (__VA_ARGS__)
#define GL_PROC_DEF(function) GL_PROC_##function function
#define GL_PROC_ADDR(function) (GL_PROC_##function)glXGetProcAddress((const GLubyte*)#function)

GL_PROC_DEC(glGenVertexArrays, GLsizei n, GLuint* arrays);
GL_PROC_DEC(glBindVertexArray, GLuint array);
GL_PROC_DEC(glGenBuffers, GLsizei n, GLuint* buffers);
GL_PROC_DEC(glBindBuffer, GLenum target, GLuint buffer);
GL_PROC_DEC(glBufferData, GLenum target, GLsizeiptr size, const GLvoid* data, GLenum usage);
GL_PROC_DEC(glEnableVertexAttribArray, GLuint index);
GL_PROC_DEC(glVertexAttribPointer, GLuint index, GLuint size, GLenum type, GLboolean normalized, GLsizei stride, const GLvoid* pointer);

GL_PROC_DEF(glGenVertexArrays);
GL_PROC_DEF(glBindVertexArray);
GL_PROC_DEF(glGenBuffers);
GL_PROC_DEF(glBindBuffer);
GL_PROC_DEF(glBufferData);
GL_PROC_DEF(glEnableVertexAttribArray);
GL_PROC_DEF(glVertexAttribPointer);

typedef struct
{
    GLuint vao;
    GLuint vbo;
} mesh_buffer;

void load_gl_functions()
{
    glGenVertexArrays = GL_PROC_ADDR(glGenVertexArrays);
    glBindVertexArray = GL_PROC_ADDR(glBindVertexArray);
    glGenBuffers = GL_PROC_ADDR(glGenBuffers);
    glBindBuffer = GL_PROC_ADDR(glBindBuffer);
    glBufferData = GL_PROC_ADDR(glBufferData);
    glEnableVertexAttribArray = GL_PROC_ADDR(glEnableVertexAttribArray);
    glVertexAttribPointer = GL_PROC_ADDR(glVertexAttribPointer);
}

mesh_buffer load_triangle()
{
    mesh_buffer mesh;
    
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    mesh.vao = vao;

    GLuint vbo;
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

    load_gl_functions();

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
