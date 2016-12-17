// window.c

#include <stdlib.h>
#include <stdio.h>

#include <X11/Xlib.h>
#include <GL/gl.h>
#include <GL/glx.h>

#include "window.h"

window_and_gl_context
create_window_and_gl_context(int width, int height, char* title)
{
    Display* display = XOpenDisplay(NULL);

    if(display == NULL)
    {
	printf("couldn't connect to X server.\n");
	exit(0);
    }

    Window root_window = DefaultRootWindow(display);

    GLint attributes[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };

    XVisualInfo* visual_info = glXChooseVisual(display, 0, attributes);
    if(visual_info == NULL)
    {
	printf("no appropriate visual available.\n");
	exit(0);
    }

    Colormap color_map = XCreateColormap(display, root_window, visual_info->visual, AllocNone);

    XSetWindowAttributes set_window_attributes;
    set_window_attributes.colormap = color_map;
    set_window_attributes.event_mask = ExposureMask | KeyPressMask | StructureNotifyMask;

    Window window = XCreateWindow(display, root_window, 0, 0, width, height, 0, visual_info->depth, InputOutput,
				  visual_info->visual, CWColormap | CWEventMask, &set_window_attributes);

    XMapWindow(display, window);

    XStoreName(display, window, title);

    GLXContext gl_context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
    glXMakeCurrent(display, window, gl_context);

    window_and_gl_context window_context;
    window_context.display = display;
    window_context.window = window;
    window_context.gl_context = gl_context;

    return window_context;
}

void
destroy_gl_context(window_and_gl_context* window_context)
{
    glXMakeCurrent(window_context->display, None, NULL);
    glXDestroyContext(window_context->display, window_context->gl_context);
}

void
destroy_window(window_and_gl_context* window_context)
{
    XDestroyWindow(window_context->display, window_context->window);
}

void
resize_viewport(window_and_gl_context* window_context)
{
    XWindowAttributes window_attributes;
    XGetWindowAttributes(window_context->display, window_context->window, &window_attributes);
    glViewport(0, 0, window_attributes.width, window_attributes.height);
}

void
redraw_window(window_and_gl_context* window_context)
{
    glXSwapBuffers(window_context->display, window_context->window);
}
