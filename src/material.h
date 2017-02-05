#ifndef MATERIAL_H_INCLUDED
#define MATERIAL_H_INCLUDED

#include "shader.h"

typedef struct material material;
typedef struct material_data material_data;

typedef enum
{
    material_data_type_unknown,
    material_data_type_vector2,
    material_data_type_vector3,
    material_data_type_vector4,
    material_data_type_matrix4,
    material_data_type_texture
} material_data_type;

struct material_data
{
    uint32 length;
    uint32 name_hash;

    material_data_type value_type;
    uint32             value_size;
};

struct material
{
    shader_program* shader;

    uint32 data_capacity;
    uint32 data_size;
    uint8* data_list;
};

material material_create(shader_program* shader, uint32 capacity);

material material_copy(material* original);
void material_free(material* material);

void material_set_named_value(material* material, char* name, void* value,
			      material_data_type type, uint32 size);
void material_set_value(material* material, uint32 name_hash, void* value,
			material_data_type type, uint32 size);

void material_set_texture(material* material, char* name, loaded_texture* texture);
void material_set_color(material* material, char* name, vector3 color);

void material_apply(material* material, shader_uniform_group* uniforms);

#endif // MATERIAL_H_INCLUDED
