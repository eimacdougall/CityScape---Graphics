#include "district.h"

inline float distanceXZ(const glm::vec3& a, const glm::vec3& b) {
    float dx = a.x - b.x;
    float dz = a.z - b.z;
    return std::sqrt(dx * dx + dz * dz);
}

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
    std::uniform_real_distribution<float> offsetDist(-4.0f, 4.0f);

    int gridX = gridDist(gen);
    int gridZ = gridDist(gen);

    //Footprint per building: buildingWidthMax + minGap
    float buildingFootprintX = (buildingWidthMax + minBuildingGap);
    float buildingFootprintZ = (buildingDepthMax + minBuildingGap);

    float maxJitterX = buildingWidthMax * 0.15f;  //Matches shader jitter
    float maxJitterZ = buildingDepthMax * 0.15f;
    float blockPadding = 20.0f;

    //Minimum and maximum distance between block origins
    float minSpacing = buildingWidthMax + 2.0f * maxJitterX + blockPadding;

    const int maxRetries = 5; //Max attempts to place a block at a valid position

    for (int z = 0; z < gridZ; z++) {
        for (int x = 0; x < gridX; x++) {
            int blockWidth = blockDist(gen);
            int blockDepth = blockDist(gen);

            float blockSpacingX = blockWidth * buildingFootprintX + 2.0f * maxJitterX + blockPadding;
            float blockSpacingZ = blockDepth * buildingFootprintZ + 2.0f * maxJitterZ + blockPadding;

            glm::vec3 origin(
                x * blockSpacingX,
                0.0f,
                z * blockSpacingZ
            );

            origin += cityOrigin;

            bool placed = false;
            for (int retry = 0; retry < maxRetries; ++retry) {
                bool valid = true;
                for (auto& b : blocks) {
                    float dist = distanceXZ(origin, b.origin);
                    if (dist < minSpacing) {
                        valid = false;
                        break;
                    }
                }

                if (valid) {
                    blocks.push_back({ origin, blockWidth, blockDepth });
                    placed = true;
                    break;
                } else {
                    //Try a new random offset within a range
                    origin.x += offsetDist(gen);
                    origin.z += offsetDist(gen);
                }
            }

            //If still not placed after retries skip
            if (!placed) continue;
        }
    }

    return blocks;
}