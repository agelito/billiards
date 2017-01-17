#ifndef WINDOW_WIN32_H_INCLUDED
#define WINDOW_WIN32_H_INCLUDED

typedef struct window_win32 window_win32;

struct window_win32
{
    HWND handle;
    HGLRC gl_context;
    int is_open;
    int width;
    int height;
};

window_win32*
window_win32_create(int width, int height, char* title);

void
window_win32_destroy(window_win32* window);

void
window_win32_process_messages(window_win32* window);

#endif // WINDOW_WIN32_H_INCLUDED
