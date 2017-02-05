// text.frag

uniform sampler2D main_texture;

uniform vec3 tint_color;

varying vec2 uv;

void main()
{
    vec4 page_color = texture2D(main_texture, uv);

    vec3 color = tint_color.rgb;
    color.r *= page_color.r;
    color.g *= page_color.r;
    color.b *= page_color.r;
    
    gl_FragColor = vec4(color.r, color.g, color.b, page_color.r);
}
