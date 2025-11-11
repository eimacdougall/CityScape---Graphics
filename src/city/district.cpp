#include "district.h"

std::vector<CityBlock> District::generateRandomCity(
    int minGridSize,
    int maxGridSize,
    int minBlockSize,
    int maxBlockSize,
    float buildingWidthMax,
    float buildingDepthMax,
    float minBuildingGap,
    const glm::vec3& cityOrigin
) {
    std::vector<CityBlock> blocks;

    //Set up random generators
    std::random_device rd;
    std::mt19937 gen(rd());
    
    //Pick values for city grid size and block sizes
    std::uniform_int_distribution<> gridDist(minGridSize, maxGridSize);
    std::uniform_int_distribution<> blockDist(minBlockSize, maxBlockSize);

    int gridX = gridDist(gen);
    int gridZ = gridDist(gen);

    for (int z = 0; z < gridZ; z++) {
        for (int x = 0; x < gridX; x++) {
            int blockWidth = blockDist(gen);
            int blockDepth = blockDist(gen);

            float blockSpacingX = blockWidth * buildingWidthMax + minBuildingGap * (blockWidth + 1);
            float blockSpacingZ = blockDepth * buildingDepthMax + minBuildingGap * (blockDepth + 1);

            glm::vec3 origin(
                x * blockSpacingX,
                0.0f,
                z * blockSpacingZ
            );

            origin += cityOrigin;

            blocks.push_back({ origin, blockWidth, blockDepth });
        }
    }

    return blocks;
}