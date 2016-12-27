// simple.vert

in vec3 in_vertex;
in vec3 in_color;

uniform mat4 world;
uniform mat4 view;
uniform mat4 projection;

varying vec3 color;

void main()
{
    color = in_color;

    mat4 world_view_projection = projection * view * world;
    gl_Position = world_view_projection * vec4(in_vertex, 1.0);
}
