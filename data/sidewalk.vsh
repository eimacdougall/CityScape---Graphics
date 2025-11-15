in vec3 a_position;

uniform mat4 u_viewProj;

void main() {
    gl_Position = u_viewProj * vec4(a_position, 1.0);
}