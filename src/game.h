#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#define assert(expr) if(!(expr)) { platform_log("assertion! %s:%d\n", __FILE__, __LINE__); *(int*)0 = 0; }

#define array_count(array) (sizeof(array) / sizeof(array[0]))
#define for_range(n, count) for(n = 0; n < count; ++n)

#define KB(kilo_bytes) (kilo_bytes * 1024)
#define MB(mega_bytes) (KB(mega_bytes) * 1024)
#define GB(giga_bytes) (MB(giga_bytes) * 1024)

#include "math.h"
#include "font.h"
#include "rendering/renderer.h"

#include "input/mouse.h"
#include "input/keyboard.h"

typedef struct game_state game_state;

struct game_state
{
    int should_quit;

    real32 time_wall;
    real32 time_frame;
    
    mouse_state mouse;
    keyboard_state keyboard;

    int screen_width;
    int screen_height;
    
    int initialized;
    
    gl_functions gl;

    shader_program textured;
    shader_program colored;
    shader_program text;
    shader_program visualize_colors;
    shader_program visualize_normals;
    shader_program visualize_texcoords;
    shader_program visualize_tangents;
    shader_program lightning;

    render_queue render_queue;

    loaded_mesh ground;
    loaded_mesh quad;
    loaded_mesh sphere;

    loaded_texture checker;
    loaded_texture checker2;
    loaded_texture table_cloth;

    loaded_texture billiard_balls[16];

    loaded_font deja_vu;

    material ground_material;
    material text_background;
    material sphere_material;
    material billiard[16];
    material pool_table;

    vector3 camera_position;
    vector3 camera_pitch_yaw_roll;
    vector3 camera_forward;
};

void
game_update_and_render(struct game_state* state);

#endif // GAME_H_INCLUDED
