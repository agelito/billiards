// texture.c

#include "opengl.h"
#include "texture.h"

#include <stdlib.h>

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
	char* pixel = (data.colors + i * pixel_size);

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

void
texture_data_free(texture_data* data)
{
    if(data && data->colors)
    {
	free(data->colors);
	data->colors = 0;
    }
}
