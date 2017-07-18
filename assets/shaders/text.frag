// text.frag
#version 330

uniform sampler2D main_texture;
uniform vec4 tint_color;

in vec2 uv;

out vec4 out_color;

void main()
{
    vec4 page_color = texture2D(main_texture, uv);

    vec4 color = tint_color;
    
    out_color = vec4(color.r, color.g, color.b, page_color.r * color.a);
}
