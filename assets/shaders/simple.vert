// simple.vert
#version 330

in vec3 in_vertex;
in vec3 in_normal;
in vec3 in_color;
in vec2 in_uv;
in vec3 in_tangent;
in vec3 in_binormal;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;
uniform vec3 tint_color;

out vec3 position;
out vec3 color;
out vec3 normal;
out vec2 uv;
out vec3 tangent;
out vec3 binormal;

void main()
{
    position = vec3(world * vec4(in_vertex, 1.0));

    mat4 normal_matrix = transpose(inverse(world));
    
    normal   = vec3(normal_matrix * vec4(in_normal, 0.0));
    tangent  = vec3(world * vec4(in_tangent, 0.0));
    binormal = vec3(world * vec4(in_binormal, 0.0));

    color = in_color;
    uv    = in_uv;
    
    mat4 world_view_projection = projection * view * world;
    gl_Position = world_view_projection * vec4(in_vertex, 1.0);
}
