in vec3 a_position;

uniform mat4 u_viewProj;

//GUI uniforms
uniform float u_spacing;
uniform float u_buildingScaleMin;
uniform float u_buildingScaleMax;
uniform float u_buildingWidthMin;
uniform float u_buildingWidthMax;
uniform float u_buildingDepthMin;
uniform float u_buildingDepthMax;

//Block info
uniform vec3 u_blockOffset;
uniform float u_gridW;
uniform float u_minBuildingGap;

out vec4 v_color;

//Hash-based pseudo-random generator
float hash(vec3 p3) {
    p3 = fract(p3 * 0.3183099 + vec3(0.1,0.2,0.3));
    p3 += dot(p3, p3.yzx + 19.19);
    return fract((p3.x + p3.y) * p3.z);
}

float randID(int id, float seed) {
    return hash(vec3(float(id) * 0.37, float(id) * seed, u_blockOffset.x + u_blockOffset.z));
}

void main() {
    int id = gl_InstanceID;
    int w = int(u_gridW);   //Cast to integer for correct indexing

    //Compute grid indices for this cube in the block
    int xIndex = id % w;
    int zIndex = id / w;

    //Randomized building size
    float width  = u_buildingWidthMin + randID(id, 1.11) * (u_buildingWidthMax - u_buildingWidthMin);
    float depth  = u_buildingDepthMin + randID(id, 1.31) * (u_buildingDepthMax - u_buildingDepthMin);
    float height = u_buildingScaleMin + randID(id, 0.93) * (u_buildingScaleMax - u_buildingScaleMin);

    //Per cube spacing
    float spacingX = u_buildingWidthMax + u_minBuildingGap;
    float spacingZ = u_buildingDepthMax + u_minBuildingGap;

    //Optional jitter within safe bounds
    float jitterFactor = 0.25;
    float maxJitterX = width * jitterFactor;
    float maxJitterZ = depth * jitterFactor;
    float offsetX = float(xIndex) * spacingX + (randID(id, 0.37) - 0.5) * maxJitterX;
    float offsetZ = float(zIndex) * spacingZ + (randID(id, 0.71) - 0.5) * maxJitterZ;

    //Apply per-vertex scaling
    vec3 pos = a_position;
    pos.x *= width;
    pos.y = (pos.y + 0.5) * height;
    pos.z *= depth;

    //Apply block origin offset
    pos.x += offsetX + u_blockOffset.x;
    pos.z += offsetZ + u_blockOffset.z;

    //Color variation
    v_color = vec4(
        0.4 + randID(id, 0.13) * 0.6,
        0.3 + randID(id, 0.57) * 0.7,
        0.5 + randID(id, 0.97) * 0.5,
        1.0
    );

    gl_Position = u_viewProj * vec4(pos, 1.0);
}