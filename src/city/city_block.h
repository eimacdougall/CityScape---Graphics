#pragma once
#include <glm/glm.hpp>

struct CityBlock {
    glm::vec3 origin;  // World-space position
    int width;         // Buildings along X
    int depth;         // Buildings along Z
};