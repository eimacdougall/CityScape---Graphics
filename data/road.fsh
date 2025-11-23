in float roadAlpha;

uniform vec3 u_color;

out vec4 PixelColor;

void main()
{
    if (roadAlpha <= 0.0)
        discard;
    PixelColor = vec4(u_color, 1.0 * roadAlpha);
}