// material.c

#include "material.h"

material
material_create(shader_program* shader, uint32 capacity)
{
    material created_material = (material){0};

    created_material.shader = shader;
    
    created_material.data_capacity = capacity;
    created_material.data_size = 0;

    created_material.data_list = (uint8*)malloc(capacity);
    memset(created_material.data_list, 0, capacity);

    return created_material;
}

material
material_copy(material* original)
{
    material copied_material = *original;

    copied_material.data_list = (uint8*)malloc(original->data_capacity);
    memcpy(copied_material.data_list, original->data_list, original->data_capacity);

    return copied_material;
}

void
material_free(material* material)
{
    material->data_capacity = 0;
    material->data_size = 0;
    if(material->data_list)
    {
	free(material->data_list);
	material->data_list = 0;
    }
}

void
material_set_named_value(material* material, char* name, void* value,
			 material_data_type type, uint32 size)
{
    uint32 name_hash = hash_string(name);
    material_set_value(material, name_hash, value, type, size);
}

void
material_set_value(material* material, uint32 name_hash, void* value,
		   material_data_type type, uint32 size)
{
    bool32 add_new = 1;
    
    uint32 data_offset = 0;
    while(data_offset < material->data_size)
    {
	material_data* data =
	    (material_data*)(material->data_list + data_offset);

	if(data->name_hash == name_hash)
	{
	    data->value_type = type;
	    
	    uint8* data_value = (uint8*)(data + 1);
	    if(size > data->value_size)
	    {
		size = data->value_size;
	    }
	    
	    memcpy(data_value, value, size);

	    add_new = 0;
	    
	    break;
	}

	data_offset += data->length;
    }

    if(!add_new) return;
    
    uint32 size_left = (material->data_capacity - material->data_size);
    uint32 data_size_required = (size + sizeof(material_data));
    if(data_size_required <= size_left)
    {
	material_data* added_data =
	    (material_data*)(material->data_list + material->data_size);
	
	added_data->length = data_size_required;
	added_data->name_hash = name_hash;
	added_data->value_type = type;
	added_data->value_size = size;

	uint8* data_location = (uint8*)(added_data + 1);
	memcpy(data_location, value, size);

	material->data_size += data_size_required;
    }
    else
    {
	platform_log("can't set material data:\n %d bytes required but only %d bytes available.\n",
		     data_size_required, size_left);
    }
}

void
material_set_texture(material* material, char* name, loaded_texture* texture)
{
    material_set_named_value(material, name, texture, material_data_type_texture,
			     sizeof(loaded_texture));
}

void
material_set_color(material* material, char* name, vector3 color)
{
    material_set_named_value(material, name, &color, material_data_type_vector3,
			     sizeof(vector3));
}

void
material_apply(material* material, shader_uniform_group* uniforms)
{
    int texture_slot = 0;
    
    uint32 data_offset = 0;
    while(data_offset < material->data_size)
    {
	material_data* data =
	    (material_data*)(material->data_list + data_offset);

	uint8* data_value = (uint8*)(data + 1);

	if(data->value_type == material_data_type_unknown)
	{
	    platform_log("material data %d, unknown data type\n", data->name_hash);
	}
	else if(data->value_type == material_data_type_texture)
	{
	    loaded_texture* texture = (loaded_texture*)data_value;

	    glActiveTexture(GL_TEXTURE0 + texture_slot);

	    if(texture)
	    {
		glBindTexture(GL_TEXTURE_2D, texture->handle);
	    }
	    else
	    {
		glBindTexture(GL_TEXTURE_2D, 0);
	    }

	    shader_uniform_set_data(uniforms, data->name_hash,
				    &texture_slot, sizeof(texture_slot));

	    texture_slot++;
	}
	else
	{
	    shader_uniform_set_data(uniforms, data->name_hash, data_value, data->value_size);
	}

	data_offset += data->length;
    }
}
