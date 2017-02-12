// text.frag

uniform sampler2D main_texture;

uniform vec4 tint_color;

varying vec2 uv;

void main()
{
    vec4 page_color = texture2D(main_texture, uv);

    vec4 color = tint_color;
    
    gl_FragColor = vec4(color.r, color.g, color.b, page_color.r * color.a);
}
