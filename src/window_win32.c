// window_win32.c

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <stdlib.h>

#include "window_win32.h"

static char* WINDOW_WIN32_CLASS = "window_win32-windowclass";

static LRESULT CALLBACK 
window_win32_message_callback(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    window_win32* window = (window_win32*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
    if(window == 0 || window->handle != hWnd) 
    {
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    switch(uMsg)
    {
    case WM_PAINT:
        break;
    case WM_DESTROY:
        window->is_open = 0;
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, uMsg, wParam, lParam);
    }

    return 0;
}

window_win32*
window_win32_create(int width, int height, char* title)
{
    char* window_title = title;
    char* window_class_name = WINDOW_WIN32_CLASS;

    HINSTANCE hinstance = GetModuleHandle(NULL);

    WNDCLASSEX window_class;
    window_class.cbSize = sizeof(WNDCLASSEX);
    window_class.lpfnWndProc = window_win32_message_callback;
    window_class.cbClsExtra = 0;
    window_class.cbWndExtra = 8;
    window_class.style = CS_OWNDC;
    window_class.hInstance = hinstance;
    window_class.hIcon = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_APPLICATION));
    window_class.hCursor = LoadCursor(NULL, IDC_ARROW);
    window_class.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    window_class.lpszMenuName = NULL;
    window_class.lpszClassName = window_class_name;
    window_class.hIconSm = LoadIcon(hinstance, MAKEINTRESOURCE(IDI_APPLICATION));

    if(!RegisterClassEx(&window_class))
    {
        MessageBox(NULL, "couldn't register window class", "error!", 0);
        return 0;
    }

    HWND window_handle = CreateWindow(window_class_name, window_title, WS_OVERLAPPEDWINDOW, 
                                      CW_USEDEFAULT, CW_USEDEFAULT, width, height, NULL, NULL,
                                      hinstance, NULL);

    if(!window_handle)
    {
        MessageBox(NULL, "couldn't create window", "error!", 0);
        return 0;
    }

    PIXELFORMATDESCRIPTOR pixel_format = {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,
        32,
        0, 0, 0, 0, 0, 0,
        0, 
        0, 
        0, 
        0, 0, 0, 0,
        24,
        8,
        0,
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };

    HDC device_context = GetDC(window_handle);
    int chosen_pixel_format = ChoosePixelFormat(device_context, &pixel_format);
    if(chosen_pixel_format == 0)
    {
        MessageBox(NULL, "failed to select suitable pixel format", "error!", 0);
        return 0;
    }

    if(!SetPixelFormat(device_context, chosen_pixel_format, &pixel_format))
    {
        MessageBox(NULL, "failed to set pixel format", "error!", 0);
        return 0;
    }

    HGLRC gl_context = wglCreateContext(device_context);
    wglMakeCurrent(device_context, gl_context);

    window_win32* window = (window_win32*)malloc(sizeof(window_win32));

    window->handle = window_handle;
    window->gl_context = gl_context;
    window->width = width;
    window->height = height;

    SetWindowLongPtr(window_handle, GWLP_USERDATA, (LONG_PTR)window);

    ShowWindow(window_handle, SW_SHOWNORMAL);
    UpdateWindow(window_handle);

    window->is_open = 1;

    return window;
}

void
window_win32_destroy(window_win32* window)
{
    if(window->gl_context)
    {
        HDC device_context = GetDC(window->handle);
        wglMakeCurrent(device_context, 0);
        wglDeleteContext(window->gl_context);
    }

    if(window->handle)
    {
        DestroyWindow(window->handle);
    }

    free(window);
}

void
window_win32_process_messages(window_win32* window)
{
    MSG message;
    while(PeekMessage(&message, window->handle, 0, 0, PM_REMOVE))
    {
        TranslateMessage(&message);
        DispatchMessage(&message);
    }
}
