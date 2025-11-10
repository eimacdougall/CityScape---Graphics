in vec3 a_position;

uniform mat4 u_viewProj;

// GUI uniforms
uniform float u_spacing;
uniform float u_buildingScaleMin;
uniform float u_buildingScaleMax;
uniform float u_buildingWidthMin;
uniform float u_buildingWidthMax;
uniform float u_buildingDepthMin;
uniform float u_buildingDepthMax;

// Block info
uniform vec3 u_blockOffset;
uniform float u_gridW;
uniform float u_minBuildingGap;

out vec4 v_color;

float rand(float n) { return fract(sin(n) * 43758.5453); }

void main() {
    int id = gl_InstanceID;

    float x = mod(float(id), u_gridW);
    float z = floor(float(id) / u_gridW);

    // Base grid spacing (adds guaranteed gap between buildings)
    float baseSpacing = u_spacing + u_minBuildingGap;

    // Randomized small jitter (% of gap between buildings)
    float jitterRange = u_minBuildingGap * 0.50;
    float offsetX = (x - u_gridW / 2.0) * baseSpacing + (rand(float(id)*0.37) - 0.5) * 2.0 * jitterRange;
    float offsetZ = (z - u_gridW / 2.0) * baseSpacing + (rand(float(id)*0.71) - 0.5) * 2.0 * jitterRange;

    // Randomized dimensions
    float width  = u_buildingWidthMin + rand(float(id)*1.11)*(u_buildingWidthMax - u_buildingWidthMin);
    float depth  = u_buildingDepthMin + rand(float(id)*1.31)*(u_buildingDepthMax - u_buildingDepthMin);
    float height = u_buildingScaleMin + rand(float(id)*0.93)*(u_buildingScaleMax - u_buildingScaleMin);

    // Build local position
    vec3 pos = a_position;
    pos.x *= width;
    pos.y = (pos.y + 0.5) * height;
    pos.z *= depth;

    // Apply grid and noise offsets
    pos.x += offsetX + u_blockOffset.x;
    pos.z += offsetZ + u_blockOffset.z;

    // Color variation
    v_color = vec4(rand(float(id)*0.13), rand(float(id)*0.57), rand(float(id)*0.97), 1.0);

    gl_Position = u_viewProj * vec4(pos, 1.0);
}