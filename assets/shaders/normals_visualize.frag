// normals_visualize.frag
#version 330

in vec3 normal;

out vec4 out_color;

void main()
{
    vec3 normal_color;
    normal_color.x = (normal.x + 1.0) * 0.5;
    normal_color.y = (normal.y + 1.0) * 0.5;
    normal_color.z = (normal.z + 1.0) * 0.5;
    out_color = vec4(normal_color, 1.0f);
}
