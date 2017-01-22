// texture.c

#include "opengl.h"
#include "texture.h"				
#include "racera.h"
#include "platform.h"

#include <stdlib.h>

#include "tga_loader.c"

loaded_texture
load_texture(gl_functions* gl, texture_data data)
{
    loaded_texture texture;

    GLuint handle;
    glGenTextures(1, &handle);
    glBindTexture(GL_TEXTURE_2D, handle);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, data.width, data.height,
		 0, GL_RGBA, GL_UNSIGNED_BYTE, data.colors);
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

    assert(components_per_pixel != 0);

    texture.width = header.width;
    texture.height = header.height;
    
    int pixel_size = components_per_pixel * sizeof(char);
    texture.colors = malloc(pixel_size * header.width * header.height);

    tga_decode_image(&header, image_data, texture.colors);
    
    platform_free_file(&file);

    return texture;
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
