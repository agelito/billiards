#ifndef TEXTURE_H_INCLUDED
#define TEXTURE_H_INCLUDED

typedef struct
{
    int width;
    int height;
    uint8 components;
    uint8* colors;
} texture_data;

typedef struct
{
    GLuint handle;
    texture_data data;
} loaded_texture;

typedef enum
{
    TEXTURE_REPEAT,
    TEXTURE_MIRROR,
    TEXTURE_CLAMP
} texture_wrap_mode;

loaded_texture load_texture(gl_functions* gl, texture_data data, texture_wrap_mode wrap_mode);

texture_data texture_create_checker(int width, int height, int checker_size);
texture_data texture_create_from_tga(char* path);

vector4 texture_unpack_bw(uint8* color);
vector4 texture_unpack_rgb(uint8* color);
vector4 texture_unpack_rgba(uint8* color);

vector4 texture_bilinear_sample(float tx, float ty, texture_data texture);

void texture_data_free(texture_data* data);

#endif // TEXTURE_H_INCLUDED
