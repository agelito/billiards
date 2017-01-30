#ifndef RACERA_H_INCLUDED
#define RACERA_H_INCLUDED

#define assert(expr) if(!(expr)) { fprintf(stderr, "assertion! %s:%d\n", __FILE__, __LINE__); *(int*)0 = 0; }

#define array_count(array) (sizeof(array) / sizeof(array[0]))
#define for_range(n, count) for(n = 0; n < count; ++n)

#define KB(kilo_bytes) (kilo_bytes * 1024)
#define MB(mega_bytes) (KB(mega_bytes) * 1024)
#define GB(giga_bytes) (MB(giga_bytes) * 1024)

#include "math.h"
#include "renderer.h"

#include "mouse.h"
#include "keyboard.h"

#define MAX_CUBES 2048

typedef struct game_state game_state;

struct game_state
{
    int should_quit;
    
    mouse_state mouse;
    keyboard_state keyboard;

    int screen_width;
    int screen_height;
    
    int initialized;
    
    gl_functions gl;
    shader_program simple_shader;
    shader_program visualize_colors;
    shader_program visualize_normals;
    shader_program visualize_texcoords;

    render_queue render_queue;

    loaded_mesh ground;
    loaded_mesh cube;
    loaded_mesh triangle;
    loaded_mesh pointer;
    loaded_mesh cup;

    loaded_texture checker;
    loaded_texture smiley;

    vector3 camera_position;
    vector3 camera_pitch_yaw_roll;
    vector3 camera_forward;
    
    int created_cube_count;
    vector3 created_cube_positions[MAX_CUBES];
};

void
game_update_and_render(struct game_state* state);

#endif // RACERA_H_INCLUDED
