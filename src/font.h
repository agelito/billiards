#ifndef FONT_H_INCLUDED
#define FONT_H_INCLUDED

typedef struct font_character font_character;
typedef struct font_kerning font_kerning;

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

struct loaded_font
{
    char* name;
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

loaded_font
font_load(char* path);

void
font_unload(loaded_font* font);

font_character*
font_get_character(loaded_font* font, uint32 character);

font_kerning*
font_get_kerning(loaded_font* font, uint32 first, uint32 second);

vector2
font_measure_text(loaded_font* font, real32 size, char* text, bool32 kerning_enabled);

#endif // FONT_H_INCLUDED
