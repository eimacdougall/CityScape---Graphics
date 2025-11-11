#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/OrbitCamera.h"
#include <random> 
#include "../shapes/cube.h"
#include "district.h"
#include "city_block.h"

class CityBuildings {
public:
    CityBuildings(wolf::App* pApp, GLuint sharedProgram = 0);
    ~CityBuildings();

    void init();
    void update(float dt);
    void render(const glm::mat4& viewProj);

    void removeLastBlock();
    void clearBlocks();
    void generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize);

    //Setters
    void setShaderProgram(GLuint program) { m_program = program; }
    void setCityOrigin(const glm::vec3& origin) { m_cityOrigin = origin; }
    void setMinBuildingGap(float gap) { m_minBuildingGap = gap; }
    void setSpacing(float s) { m_blockSpacing = s; }
    void setBuildingHeightRange(float minH, float maxH) { m_buildingScaleMin = minH; m_buildingScaleMax = maxH; }
    void setBuildingSizeRange(float minW, float maxW, float minD, float maxD) {
        m_buildingWidthMin = minW; m_buildingWidthMax = maxW;
        m_buildingDepthMin = minD; m_buildingDepthMax = maxD;
    }

    GLuint getProgram() const { return m_program; }

private:
    wolf::App* m_pApp = nullptr;

    GLuint m_program = 0; //Can be shared across instances
    Cube m_cube;
    District m_district;

    //Uniform locations
    GLint m_uViewProjLoc = -1;
    GLint m_uSpacingLoc = -1;
    GLint m_uMinHeightLoc = -1;
    GLint m_uMaxHeightLoc = -1;
    GLint m_uWidthMinLoc = -1;
    GLint m_uWidthMaxLoc = -1;
    GLint m_uDepthMinLoc = -1;
    GLint m_uDepthMaxLoc = -1;
    GLint m_uBlockOffsetLoc = -1;
    GLint m_uGridWLoc = -1;
    GLint m_uMinBuildingGapLoc = -1;

    //Parameters
    float m_blockSpacing = 1.0f; //Distance between blocks
    float m_minBuildingGap = 2.5f; //Minimum gap between buildings in a block
    float m_buildingScaleMin = 1.0f;
    float m_buildingScaleMax = 5.0f;

    float m_buildingWidthMin = 1.0f;
    float m_buildingWidthMax = 3.0f;
    float m_buildingDepthMin = 1.0f;
    float m_buildingDepthMax = 3.0f;

    std::vector<CityBlock> m_blocks;
    glm::vec3 m_cityOrigin = glm::vec3(0.0f);
};