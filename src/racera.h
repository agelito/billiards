#ifndef RACERA_H_INCLUDED
#define RACERA_H_INCLUDED

#define UNUSED(variable) (void)(variable)

#define invalid_code *((int*)0) = 0
#define assert(expr) if(!(expr)) { fprintf(stderr, "assertion! %s:%d\n", __FILE__, __LINE__); invalid_code; }
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#define for_range(n, count) for(n = 0; n < count; ++n)

#define KB(kilo_bytes) (kilo_bytes * 1024)
#define MB(mega_bytes) (KB(mega_bytes) * 1024)
#define GB(giga_bytes) (MB(giga_bytes) * 1024)

#include "opengl.h"
#include "math.h"
#include "renderer.h"

#include "texture.h"

#include "mouse.h"
#include "keyboard.h"

#define MAX_CUBES 2048

typedef struct
{
    vector3 position;
    float yaw;
    float pitch;
} fps_camera;

typedef struct
{
    int should_quit;
    
    mouse_state mouse;
    keyboard_state keyboard;

    int screen_width;
    int screen_height;
    
    int loaded_graphics;
    
    gl_functions gl;
    shader_program simple_shader;
    shader_program uv_shader;

    shader_uniform_group per_scene_uniforms;
    shader_uniform_group per_object_uniforms;

    render_queue render_queue;

    loaded_mesh ground;
    loaded_mesh cube;
    loaded_mesh pointer;

    loaded_texture texture;
    loaded_texture smiley;

    fps_camera camera;
    
    int created_cube_count;
    vector3 created_cube_positions[MAX_CUBES];
} game_state;

void game_update_and_render(game_state* state);

#endif // RACERA_H_INCLUDED
