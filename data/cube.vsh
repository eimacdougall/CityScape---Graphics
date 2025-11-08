in vec3 a_position;
in vec2 a_gridOffset;
in vec3 a_instancePosition;
in vec3 a_instanceRotation;
in float a_instanceScale;
in vec4 a_instanceColor;

out vec4 v_color;

uniform mat4 u_viewProj;

//Each rotation matrix will return a mat4 in radians
mat4 rotateX(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        1.0, 0.0, 0.0, 0.0,
        0.0, c,   -s,  0.0,
        0.0, s,   c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateY(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,   0.0, s,   0.0,
        0.0, 1.0, 0.0, 0.0,
        -s,  0.0, c,   0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 rotateZ(float angle) {
    float c = cos(angle);
    float s = sin(angle);
    return mat4(
        c,   -s,  0.0, 0.0,
        s,   c,   0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

void main() {
    vec3 gridPos = vec3(a_gridOffset.x, 0.0, a_gridOffset.y);
    vec3 finalPos = gridPos + a_instancePosition;
    
    vec3 scaled = a_position * a_instanceScale;
    
    mat4 rotX = rotateX(a_instanceRotation.x);
    mat4 rotY = rotateY(a_instanceRotation.y);
    mat4 rotZ = rotateZ(a_instanceRotation.z);
    vec4 rotated = rotZ * rotY * rotX * vec4(scaled, 1.0); //Combining rotations
    
    vec3 worldPos = rotated.xyz + finalPos;
    
    gl_Position = u_viewProj * vec4(worldPos, 1.0);
    v_color = a_instanceColor;
}