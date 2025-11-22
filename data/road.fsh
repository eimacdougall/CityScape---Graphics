uniform vec3 u_color;

out vec4 PixelColor;

void main()
{
    PixelColor = vec4(u_color, 1.0);
}