// win32_main.c

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include "window_win32.h"

int CALLBACK WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow)
{
    window_win32* window = window_win32_create(500, 500, "racera");

    while(window->is_open)
    {
        window_win32_process_messages(window);
    }

    window_win32_destroy(window);
    window = 0;

    return 0;

}


