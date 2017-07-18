// tangents_visualize.frag
#version 330

in vec4 tangent;

out vec4 out_color;

void main()
{
    vec3 tangent_color;
    tangent_color.x = (tangent.x + 1.0) * 0.5;
    tangent_color.y = (tangent.y + 1.0) * 0.5;
    tangent_color.z = (tangent.z + 1.0) * 0.5;
    out_color = vec4(tangent_color, 1.0f);
}
