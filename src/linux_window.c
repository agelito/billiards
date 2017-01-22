// linux_window.c

typedef struct
{
    Display* display;
    Window window;
    
    int width;
    int height;

    int pointer_is_grabbed;
    Cursor empty_cursor;
}  window_x11;

static window_x11
create_window(int width, int height, char* title)
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
    set_window_attributes.event_mask = ExposureMask | KeyPressMask | KeyReleaseMask | StructureNotifyMask | FocusChangeMask;

    Window window = XCreateWindow(display, root_window, 0, 0, width, height, 0, visual_info->depth, InputOutput,
                                  visual_info->visual, CWColormap | CWEventMask, &set_window_attributes);

    XMapWindow(display, window);

    XStoreName(display, window, title);

    XClassHint* class_hint = XAllocClassHint();
    if(class_hint)
    {
	class_hint->res_name = title;
	class_hint->res_class = title;

	XSetClassHint(display, window, class_hint);
	XFree(class_hint);
    }

    XSizeHints* size_hints = XAllocSizeHints();
    if(size_hints)
    {
	size_hints->flags = PMinSize | PMaxSize;
	size_hints->min_width = width;
	size_hints->max_width = width;
	size_hints->min_height = height;
	size_hints->max_height = height;

	XSetWMNormalHints(display, window, size_hints);
	XFree(size_hints);
    }

    XColor cursor_color = (XColor){0};
    char cursor_color_data[] = {0};

    Pixmap cursor_pixmap = XCreateBitmapFromData(display, window, cursor_color_data, 1, 1);
    Cursor empty_cursor = XCreatePixmapCursor(display, cursor_pixmap, cursor_pixmap, &cursor_color,
					      &cursor_color, 0, 0);

    XFreePixmap(display, cursor_pixmap);

    GLXContext gl_context = glXCreateContext(display, visual_info, NULL, GL_TRUE);
    glXMakeCurrent(display, window, gl_context);

    window_x11 window_context;
    window_context.display = display;
    window_context.window = window;
    window_context.empty_cursor = empty_cursor;
    window_context.pointer_is_grabbed =
	mouse_x11_grab(display, window);

    return window_context;
}

static void
destroy_current_gl_context(window_x11* window_context)
{
    GLXContext gl_context = glXGetCurrentContext();
    if(gl_context != NULL)
    {
        glXMakeCurrent(window_context->display, None, NULL);
        glXDestroyContext(window_context->display, gl_context);
    }
}


static void
destroy_window(window_x11* window_context)
{
    XFreeCursor(window_context->display, window_context->empty_cursor);
    
    destroy_current_gl_context(window_context);

    if(window_context->window)
    {
        XDestroyWindow(window_context->display, window_context->window);
        window_context->window = 0;
    }

    XCloseDisplay(window_context->display);
}

static void
resize_viewport(window_x11* window_context)
{
    XWindowAttributes window_attributes;
    XGetWindowAttributes(window_context->display, window_context->window, &window_attributes);
    glViewport(0, 0, window_attributes.width, window_attributes.height);
    window_context->width = window_attributes.width;
    window_context->height = window_attributes.height;
}

static void
redraw_window(window_x11* window_context)
{
    glXSwapBuffers(window_context->display, window_context->window);
}

static int
handle_window_events(window_x11* window_context, keyboard_x11* keyboard, xinput2* xinput)
{
    int window_is_open = 1;

    while(XPending(window_context->display))
    {
        XEvent event;
        XNextEvent(window_context->display, &event);

	if(xinput2_process_event(window_context->display, xinput, &event))
	{

	}
	else
	{
	    if(event.type == KeyPress)
	    {
		keycode_state* state = (keyboard->state + event.xkey.keycode);
		if(!state->down)
		{
		    state->down = 1;
		    state->pressed = 1;
		}
	    }
	    else if(event.type == KeyRelease)
	    {
		int was_auto_repeat = 0;
		// NOTE: Check for auto repeat
		if(XPending(window_context->display))
		{
		    XEvent next;
		    XPeekEvent(window_context->display, &next);
		    was_auto_repeat = (next.type == KeyPress && next.xkey.time == event.xkey.time && next.xkey.keycode == event.xkey.keycode);
		}

		if(!was_auto_repeat)
		{
		    keycode_state* state = (keyboard->state + event.xkey.keycode);
		    state->down = 0;
		    state->released = 1;
		}
	    }
	    else if(event.type == FocusIn)
	    {
		if(!window_context->pointer_is_grabbed)
		{
		    window_context->pointer_is_grabbed =
			mouse_x11_grab(window_context->display, window_context->window);

		    mouse_x11_set_cursor(window_context->display, window_context->empty_cursor);
		}
	    }
	    else if(event.type == FocusOut)
	    {
		if(window_context->pointer_is_grabbed)
		{
		    mouse_x11_ungrab(window_context->display);
		    window_context->pointer_is_grabbed = 0;

		    mouse_x11_set_cursor(window_context->display, None);
		}
	    }
	    else if(event.type == ConfigureNotify)
	    {
		resize_viewport(window_context);
	    }
	    else if(event.type == DestroyNotify)
	    {
		destroy_current_gl_context(window_context);
            
		window_context->window = 0;
		window_is_open = 0;
	    }
	}
    }

    return window_is_open;
}
