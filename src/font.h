#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

#include "opengl.h"
#include "texture.h"
#include "material.h"

typedef struct font_character font_character;
typedef struct font_kerning font_kerning;

typedef struct font_data font_data;

typedef struct loaded_font loaded_font;

struct font_character
{
    uint32 id;
    uint16 source_x;
    uint16 source_y;
    uint16 source_w;
    uint16 source_h;

    int16 offset_x;
    int16 offset_y;
    int16 advance;

    uint8 page;
    uint8 channel;
};

struct font_kerning
{
    uint32 first;
    uint32 second;
    int16 amount;
};

struct font_data
{
    uint16 size;

    int16 line_height;
    int16 baseline;

    uint16 page_count;
    char** page_names;

    uint32 character_count;
    font_character* characters;

    uint32 kerning_count;
    font_kerning* kerning;
};

struct loaded_font
{
    font_data data;

    loaded_texture* textures;
    material* materials;
};

loaded_font
load_font(gl_functions* gl, font_data data);

font_data
font_create_from_file(char* path);

font_character*
font_get_character(font_data* font, uint32 character);

font_kerning*
font_get_kerning(font_data* font, uint32 first, uint32 second);

vector2
font_measure_text(font_data* font, real32 size, char* text, bool32 kerning_enabled);

void
font_data_free(font_data* font);

#endif // FONT_H_INCLUDED
