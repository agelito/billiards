// font.c

#include "platform.h"
#include "font.h"

#define BM_FONT_BLOCK_INFO            1
#define BM_FONT_BLOCK_COMMON          2
#define BM_FONT_BLOCK_PAGES           3
#define BM_FONT_BLOCK_CHARS           4
#define BM_FONT_BLOCK_KERNING_PAIRS   5

#define BM_FONT_SMOOTH_BIT            0x01
#define BM_FONT_UNICODE_BIT           0x02
#define BM_FONT_ITALIC_BIT            0x04
#define BM_FONT_BOLD_BIT              0x08
#define BM_FONT_FIXED_HEIGHT_BIT      0x10

loaded_font
font_load(char* path)
{
    platform_log("loading font definition %s\n", path);

    loaded_font font = (loaded_font){0};

    read_file definition_file = platform_read_file(path, 1);
    if(definition_file.size == 0)
    {
        platform_log("%s doesn't exist or is empty\n", path);
        return font;
    }

    if(definition_file.size <= 4)
    {
        platform_log("%s invalid file format\n", path);
        return font;
    }

    char* file_header = (char*)definition_file.data;
    if(file_header[0] != 66 || file_header[1] != 77 || file_header[2] != 70 || file_header[3] != 3)
    {	
        platform_log("incorrect bmfont file header or version\n header: %c%c%c\n version: %d\n",
                     file_header[0], file_header[1], file_header[2], file_header[3]);

	platform_free_file(&definition_file);
        return font;
    }

    unsigned long read_position = 4;
    while(read_position < definition_file.size)
    {
        char* block_header = (char*)(definition_file.data + read_position);
        uint8 block_type   = *((uint8*)(block_header + 0));
        uint32 length      = *((uint32*)(block_header + 1));
	
        read_position += 5;

        char* block_data = (block_header + 5);

        char* font_name = 0;

        switch(block_type)
        {
        case BM_FONT_BLOCK_INFO: {
            font.size = *((int16*)(block_data + 0));
            font_name = (block_data + 14);
            platform_log(" %s %d\n", font_name, font.size);
	} break;
        case BM_FONT_BLOCK_COMMON: {
            font.line_height  = *((int16*)(block_data + 0));
            font.baseline     = *((int16*)(block_data + 2));
            font.page_count   = *((uint16*)(block_data + 8));

	    uint32 page_names_size = (font.page_count * sizeof(char*));
            font.page_names        = (char**)malloc(page_names_size);
	    
            if(font.page_names == 0)
            {
                platform_log("couldn't allocate %d bytes for page names\n", page_names_size);
                read_position = definition_file.size;
                break;
            }

	    uint32 page_index;
	    for_range(page_index, font.page_count)
	    {
		*(font.page_names + page_index) = 0;
	    }
	} break;
        case BM_FONT_BLOCK_PAGES: {
            uint32 path_length = (uint32)strlen(path);
            while(path_length > 0)
            {
                char previous_character = *((path + path_length) - 1);
		if(previous_character == '\\' ||
		   previous_character == '/') break;
		
                path_length--;
            }
	    
            uint32 name_offset = 0;
            uint32 name_index = 0;
            while(name_offset < length)
            {
                char* name = (char*)(block_data + name_offset);
                uint32 name_length_with_null = (uint32)strlen(name) + 1;
                uint32 path_length_with_null = path_length + name_length_with_null;
                font.page_names[name_index] = malloc(path_length_with_null);
                memcpy(font.page_names[name_index], path, path_length);
                memcpy(font.page_names[name_index] + path_length, name, name_length_with_null);
                name_offset += name_length_with_null;
            }
        } break;
        case BM_FONT_BLOCK_CHARS: {
            font.character_count = (length / 20);
            platform_log(" character count: %d\n", font.character_count);

	    uint32 characters_size = (font.character_count * sizeof(font_character));
            font.characters = (font_character*)malloc(characters_size);
	    
            if(font.characters == 0)
            {
                platform_log("couldn't allocate %d bytes for font characters\n", characters_size);
                read_position = definition_file.size;
                break;
            }

            uint32 character;
	    for_range(character, font.character_count)
            {
                uint32 character_offset = (character * 20);
		
                font_character* font_character = (font.characters + character);
                font_character->id = *((uint32*)(block_data + 0 + character_offset));
                font_character->source_x = *((uint16*)(block_data + 4 + character_offset));
                font_character->source_y = *((uint16*)(block_data + 6 + character_offset));
                font_character->source_w = *((uint16*)(block_data + 8 + character_offset));
                font_character->source_h = *((uint16*)(block_data + 10 + character_offset));
                font_character->offset_x = *((int16*)(block_data + 12 + character_offset));
                font_character->offset_y = *((int16*)(block_data + 14 + character_offset));
                font_character->advance = *((int16*)(block_data + 16 + character_offset));
                font_character->page = *((uint8*)(block_data + 18 + character_offset));
                font_character->channel = *((uint8*)(block_data + 19 + character_offset));
            }
        } break;
        case BM_FONT_BLOCK_KERNING_PAIRS:
            font.kerning_count = (length / 10);
            platform_log(" kerning count: %d\n", font.kerning_count);

	    uint32 kerning_size = (font.kerning_count * sizeof(font_kerning));
            font.kerning = (font_kerning*)malloc(kerning_size);
            if(font.kerning == 0)
            {
                platform_log("couldn't allocate %d bytes for kerning data\n", kerning_size);
                read_position = definition_file.size;
                break;
            }
            uint32 kerning;
	    for_range(kerning, font.kerning_count)
            {
                font_kerning* kerning_info = (font.kerning + kerning);
                kerning_info->first = *((uint32*)(block_data + 0 + kerning * 10));
                kerning_info->second = *((uint32*)(block_data + 4 + kerning * 10));
                kerning_info->amount = *((int16*)(block_data + 8 + kerning * 10));
            }
            break;
        default:
            platform_log("unknown block type %d at offset %ld\n", block_type, read_position);
            break;
        }
        read_position += length;
    }

    platform_free_file(&definition_file);
    
    return font;
}

void
font_unload(loaded_font* font)
{
    if(font->page_names)
    {
        int page_index;
	for_range(page_index, font->page_count)
	{
	    char* page_name = *(font->page_names + page_index);
	    if(page_name) free(page_name);
	}

        free(font->page_names);
    }

    if(font->characters) free(font->characters);

    *font = (loaded_font){0};
}

font_character*
font_get_character(loaded_font* font, uint32 character)
{
    font_character* result = 0;
    
    uint32 character_index;
    for_range(character_index, font->character_count)
    {
	font_character* check_character = (font->characters + character_index);
        if(check_character->id == character)
        {
            result = check_character;
            break;
        }
    }

    return result;
}

font_kerning*
font_get_kerning(loaded_font* font, unsigned int first, unsigned int second)
{
    font_kerning* result = 0;

    uint32 kerning_index;
    for_range(kerning_index, font->kerning_count)
    {
        font_kerning* check_kerning = (font->kerning + kerning_index);
        if(check_kerning->first == first && check_kerning->second == second)
        {
            result = check_kerning;
            break;
        }
    }

    return result;
}

vector2
font_measure_text(loaded_font* font, real32 size, char* text, bool32 kerning_enabled)
{
    int32 cursor_x = 0;
    int32 cursor_y = 0;

    float max_width = 0.0f;
    float max_height = font->line_height;
    float one_over_font_size = 1.0f / font->size;

    char current, previous = 0;
    while((current = *text++) != 0)
    {
        if(current == '\n')
        {
            if(cursor_x > max_width) max_width = (real32)cursor_x;

            max_height += font->line_height;
	    
            cursor_x = 0;
            cursor_y -= font->line_height;
            continue;
        }
	    
        font_character* character = font_get_character(font, (unsigned int)current);
        if(character == 0) continue;

        // TODO: Calculate bounds using top/bottom variables

        real32 left   = (real32)cursor_x + character->offset_x;
        real32 right  = left + character->source_w;

        int kerning_offset = 0;
	    
        if(kerning_enabled)
        {
            font_kerning* kerning = font_get_kerning(font, (uint32)previous, (uint32)current);
            if(kerning != 0) kerning_offset = kerning->amount;
        }

        left += kerning_offset;
        right += kerning_offset;

        cursor_x += (character->advance + kerning_offset);

        previous = current;
    }

    if(cursor_x > max_width) max_width = (float)cursor_x;

    vector2 result;
    result.x = (max_width * size * one_over_font_size);
    result.y = (max_height * size * one_over_font_size);

    return result;
}
