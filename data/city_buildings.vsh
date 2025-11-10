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

out vec4 v_color;

float rand(float n) { return fract(sin(n) * 43758.5453); } //Fake random generator, will improve later

void main() {
    int id = gl_InstanceID;

    float x = mod(float(id), u_gridW);
    float z = floor(float(id) / u_gridW);

    float offsetX = (x - u_gridW/2.0) * u_spacing + (rand(float(id)*0.37)-0.5)*3.0; //Adding some noise to building positions, would like to improve so that 
    float offsetZ = (z - u_gridW/2.0) * u_spacing + (rand(float(id)*0.71)-0.5)*3.0; //it isn't the same for all blocks

    float width  = u_buildingWidthMin + rand(float(id)*1.11)*(u_buildingWidthMax-u_buildingWidthMin);
    float depth  = u_buildingDepthMin + rand(float(id)*1.31)*(u_buildingDepthMax-u_buildingDepthMin);
    float height = u_buildingScaleMin + rand(float(id)*0.93)*(u_buildingScaleMax-u_buildingScaleMin);

    vec3 pos = a_position;
    pos.x *= width;
    pos.y = (pos.y + 0.5) * height;
    pos.z *= depth;

    pos.x += offsetX + u_blockOffset.x;
    pos.z += offsetZ + u_blockOffset.z;

    v_color = vec4(rand(float(id)*0.13), rand(float(id)*0.57), rand(float(id)*0.97), 1.0);
    gl_Position = u_viewProj * vec4(pos,1.0);
}