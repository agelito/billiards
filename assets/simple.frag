// simple.frag

uniform sampler2D main_texture;

varying vec3 color;
varying vec2 uv;

void main()
{
    gl_FragColor = texture2D(main_texture, uv) * vec4(color, 1.0);
}
