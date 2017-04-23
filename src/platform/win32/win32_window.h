#ifndef WIN32_WINDOW_H_INCLUDED
#define WIN32_WINDOW_H_INCLUDED

typedef struct win32_window win32_window;

struct win32_window
{
    HWND handle;
    HGLRC gl_context;
    int is_open;
    int width;
    int height;
};

win32_window*
win32_window_create(int width, int height, char* title);

void
win32_window_destroy(win32_window* window);

void
win32_window_process_messages(win32_window* window);

#endif // WIN32_WINDOW_H_INCLUDED
