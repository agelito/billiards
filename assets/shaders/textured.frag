// textured.frag
#version 330

uniform sampler2D main_texture;

in vec2 uv;

out vec4 out_color;

void main()
{
    out_color = texture2D(main_texture, uv);
}
