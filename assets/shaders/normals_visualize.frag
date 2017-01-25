// normals_visualize.frag

varying vec3 normal;

void main()
{
    normal.x = (normal.x + 1.0) * 0.5;
    normal.y = (normal.y + 1.0) * 0.5;
    normal.z = (normal.z + 1.0) * 0.5;
    gl_FragColor = vec4(normal.x, normal.y, normal.z, 1.0);
}
