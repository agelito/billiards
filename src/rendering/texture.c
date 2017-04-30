// texture.c

#include "texture.h"				

#include "../asset_import/tga_importer.c"

#define TEXTURE_COLOR_POINTER(texture, x, y)			\
    texture.colors + (x + y * texture.width) * texture.components

loaded_texture
load_texture(gl_functions* gl, texture_data data)
{
    loaded_texture texture;

    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    GLenum texture_format = GL_RGB;
    if(data.components == 4)
    {
	texture_format = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, texture_format, data.width, data.height,
		 0, texture_format, GL_UNSIGNED_BYTE, data.colors);
    gl->glGenerateMipmap(GL_TEXTURE_2D);

    glBindTexture(GL_TEXTURE_2D, 0);

    texture.handle = handle;
    texture.data = data;
    
    return texture;
}

texture_data
texture_create_checker(int width, int height, int checker_size)
{
    texture_data data = (texture_data){0};

    int components_per_pixel = 4;
    int pixel_size = components_per_pixel * sizeof(char);

    data.width = width;
    data.height = height;
    data.components = components_per_pixel;
    data.colors = malloc(pixel_size * width * height);

    int i, pixels = width * height;
    for(i = 0; i < pixels; i++)
    {
	unsigned char* pixel = (data.colors + i * pixel_size);

	int checker_index = i / checker_size;
	
	int black = 0;
	int row = (checker_index / width) % 2;
	if(row)
	{
	    black = (checker_index % 2 == 0);
	}
	else
	{
	    black = !(checker_index % 2 == 0);
	}

	
	char value = 255;
	
	if(black)
	{
	    value = 0;
	}
	
	pixel[0] = value;
	pixel[1] = value;
	pixel[2] = value;
	pixel[3] = 255;
    }
    
    return data;
}

texture_data
texture_create_from_tga(char* path)
{
    texture_data texture = (texture_data){0};

    read_file file =  platform_read_file(path, 1);
    assert(file.size > sizeof(tga_header));
    
    tga_header header = *((tga_header*)file.data);
    
    unsigned char* image_data = (file.data + (sizeof(tga_header) + header.id_length));
    
    int components_per_pixel = 0;
    switch(header.bits_per_pixel)
    {
    case 8:
    case 24:
	components_per_pixel = 3;
	break;
    case 16:
    case 32:
	components_per_pixel = 4;
	break;
    }

    texture.components = components_per_pixel;

    assert(components_per_pixel != 0);

    texture.width = header.width;
    texture.height = header.height;
    
    int pixel_size = components_per_pixel * sizeof(char);
    texture.colors = malloc(pixel_size * header.width * header.height);

    tga_decode_image(&header, image_data, texture.colors);
    
    platform_free_file(&file);

    return texture;
}

vector4
texture_unpack_bw(uint8* color)
{
    vector4 result = (vector4){0};
    result.x = *(color + 0) / 255.0f;
    return result;
}

vector4
texture_unpack_rgb(uint8* color)
{
    vector4 result = (vector4){0};
    result.x = *(color + 0) / 255.0f;
    result.y = *(color + 1) / 255.0f;
    result.z = *(color + 2) / 255.0f;
    return result;
}

vector4
texture_unpack_rgba(uint8* color)
{
    vector4 result = (vector4){0};
    result.x = *(color + 0) / 255.0f;
    result.y = *(color + 1) / 255.0f;
    result.z = *(color + 2) / 255.0f;
    result.w = *(color + 3) / 255.0f;
    return result;
}

vector4
texture_bilinear_sample(float tx, float ty, texture_data texture)
{
    tx = (tx * (texture.width - 1));
    ty = (ty * (texture.height - 1));
    
    float fraction_x = tx - (int)tx;
    float fraction_y = ty - (int)ty;

    float one_minus_fx = 1.0f - fraction_x;
    float one_minus_fy = 1.0f - fraction_y;

    int x0 = (int)tx;
    int y0 = (int)ty;
    int x1 = x0 + 1;
    int y1 = y0 + 1;

    // TODO: Unpack depending on pixel type.
    vector4 colorx0y0 = texture_unpack_bw(TEXTURE_COLOR_POINTER(texture, x0, y0));
    vector4 colorx1y0 = texture_unpack_bw(TEXTURE_COLOR_POINTER(texture, x1, y0));
    vector4 colorx0y1 = texture_unpack_bw(TEXTURE_COLOR_POINTER(texture, x0, y1));
    vector4 colorx1y1 = texture_unpack_bw(TEXTURE_COLOR_POINTER(texture, x1, y1));

    vector4 a =
	vector4_add(vector4_scale(colorx0y0, one_minus_fx), vector4_scale(colorx1y0, fraction_x));
    vector4 b =
	vector4_add(vector4_scale(colorx0y1, one_minus_fx), vector4_scale(colorx1y1, fraction_x));

    return vector4_add(vector4_scale(a, one_minus_fy), vector4_scale(b, fraction_y));
}

void
texture_data_free(texture_data* data)
{
    if(data && data->colors)
    {
	free(data->colors);
	data->colors = 0;
    }
}
