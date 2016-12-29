// simple.frag

uniform sampler2D main_texture;

varying vec3 color;
varying vec2 uv;

out vec4 out_color;

void main()
{
    out_color = texture(main_texture, uv) * vec4(color, 1.0);
}
