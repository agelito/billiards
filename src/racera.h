#ifndef RACERA_H_INCLUDED
#define RACERA_H_INCLUDED

#define UNUSED(variable) (void)(variable)

#define invalid_code *((int*)0) = 0
#define array_count(array) (sizeof(array) / sizeof(array[0]))

#include "opengl.h"
#include "math.h"
#include "shader.h"
#include "mesh.h"
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
    shader_program shader;

    loaded_mesh ground;
    loaded_mesh cube;
    loaded_mesh pointer;

    loaded_texture texture;

    fps_camera camera;
    
    int created_cube_count;
    vector3 created_cube_positions[MAX_CUBES];
} game_state;

void game_update_and_render(game_state* state);

#endif // RACERA_H_INCLUDED
