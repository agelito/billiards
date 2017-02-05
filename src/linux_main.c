// linux_main.c

#define _XOPEN_SOURCE 600
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <time.h>
#undef _XOPEN_SOURCE

#include <GL/gl.h>
#include <GL/glx.h>

#include <X11/Xlib.h>
#include <X11/keysym.h>
#include <X11/extensions/XInput2.h>

#define unused_arg(arg) (void)arg;
#define array_count(array) (sizeof(array) / sizeof(array[0]))
#define invalid_code *(int*)0 = 0

#include "platform.h"

#include "linux_platform.c"
#include "linux_cpu.c"
#include "linux_mouse.c"
#include "linux_mouse_raw.c"
#include "linux_keyboard.c"
#include "linux_window.c"

#include "racera.h"

static uint64
get_precise_time()
{
    struct timespec time;
    int result = clock_gettime(CLOCK_MONOTONIC_RAW, &time);
    if(result == -1) return 0;

    return (time.tv_sec * 1000000) + (time.tv_nsec / 1000);
}

int main(int argc, char* argv[])
{
    unused_arg(argc);
    unused_arg(argv);

    char exe_dir[1024];
    int exe_dir_length = 1024;
    
    exe_dir_length = platform_executable_directory(exe_dir, exe_dir_length);
    platform_set_working_directory(exe_dir);
    
    window_x11 window_context = create_window(1280, 720, "racera");

    keyboard_x11 keyboard = keyboard_x11_init(window_context.display);
    xinput2 mouse_raw = xinput2_init(window_context.display);

    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    
    

    renderer_check_error();

    game_state racera_state = (game_state){0};

    uint64 elapsed_time = 0;
    uint64 previous_time = get_precise_time();

    while(handle_window_events(&window_context, &keyboard, &mouse_raw))
    {
	uint64 current_time = get_precise_time();
	uint64 delta_time = (current_time - previous_time);
	previous_time = current_time;
	elapsed_time += delta_time;

	racera_state.time_wall = (real32)(elapsed_time * 0.000001f);
	racera_state.time_frame = (real32)(delta_time * 0.000001f);
	
	racera_state.screen_width = window_context.width;
	racera_state.screen_height = window_context.height;

	int key;
	for(key = 0; key < VKEY_COUNT; key++)
	{
	    *(racera_state.keyboard.down + key) = keyboard_x11_is_down(&keyboard, key);
	    *(racera_state.keyboard.pressed + key) = keyboard_x11_is_pressed(&keyboard, key);
	    *(racera_state.keyboard.released + key) = keyboard_x11_is_released(&keyboard, key);
	}
	
	xinput2_mouse mouse_axis = xinput2_get_default_axis(&mouse_raw);
	mouse_apply_relative(&racera_state.mouse, mouse_axis.x, mouse_axis.y, mouse_axis.wheel);
	mouse_clamp_to_window(&racera_state.mouse, window_context.width, window_context.height);
	
	game_update_and_render(&racera_state);
	if(racera_state.should_quit)
	{
	    destroy_window(&window_context);
	    break;
	}

	redraw_window(&window_context);

	keyboard_x11_reset(&keyboard);
	xinput2_reset_axis_data(&mouse_raw);
	
	platform_sleep(1);
    }

    return 0;
}

void* gl_get_address(const GLubyte* function)
{
    return (void*)glXGetProcAddress(function);
}
