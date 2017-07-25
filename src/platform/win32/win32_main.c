// win32_main.c

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "win32_window.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    win32_window* window = win32_window_create(500, 500, "billiards");

    while(window->is_open)
    {
        win32_window_process_messages(window);
    }

    win32_window_destroy(window);
    window = 0;

    return 0;

}


