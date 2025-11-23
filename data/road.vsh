in vec3 a_position;

uniform mat4 u_viewProj;
uniform vec3 uCameraPos;
uniform float uMaxDist;

out float roadAlpha;

void main()
{
    //Compute horizontal distance (XZ plane) from camera
    vec2 posXZ = vec2(a_position.x, a_position.z);
    vec2 camXZ = vec2(uCameraPos.x, uCameraPos.z);
    float dist = distance(posXZ, camXZ);

    //Compute alpha: 1.0 near camera, 0.0 at max distance
    roadAlpha = clamp(1.0 - (dist / uMaxDist), 0.0, 1.0);

    //Transform vertex position
    gl_Position = u_viewProj * vec4(a_position, 1.0);
}