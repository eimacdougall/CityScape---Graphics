#pragma once
#include <vector>
#include <random>
#include "../wolf/wolf.h"
#include "city_block.h"  // for CityBlock definition

class District {
public:
    District() = default;
    ~District() = default;

    // Generates a random set of city blocks for this district
    std::vector<CityBlock> generateRandomCity(
        int minGridSize,
        int maxGridSize,
        int minBlockSize,
        int maxBlockSize,
        float buildingWidthMax,
        float buildingDepthMax,
        float minBuildingGap,
        const glm::vec3& cityOrigin
    );
};