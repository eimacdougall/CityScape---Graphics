#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include <random> 
#include "../shapes/cube.h"
#include "district.h"
#include "city_block.h"
#include "sidewalk.h"
#include "road.h"

class CityBuildings {
public:
    CityBuildings(wolf::App* pApp, GLuint sharedProgram, GLuint sidewalkShader, GLuint roadShader = 0);
    ~CityBuildings();

    void init();
    void update(float dt);
    void render(const glm::mat4& viewProj);

    void removeLastBlock();
    void clearBlocks();

    std::vector<CityBlock> generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize);

    void setShaderProgram(GLuint program) { m_program = program; }
    void setCityOrigin(const glm::vec3& origin) { m_cityOrigin = origin; }
    void setMinBuildingGap(float gap) { m_minBuildingGap = gap; }
    void setBuildingHeightRange(float minH, float maxH) { m_buildingScaleMin = minH; m_buildingScaleMax = maxH; }
    void setBuildingSizeRange(float minW, float maxW, float minD, float maxD) {
        m_buildingWidthMin = minW; m_buildingWidthMax = maxW;
        m_buildingDepthMin = minD; m_buildingDepthMax = maxD;
    }

    //Getters
    GLuint getProgram() const { return m_program; }
    Sidewalk& getSidewalk() { return m_sidewalk; }
    const std::vector<CityBlock>& getBlocks() const { return m_blocks; }
    float CityBuildings::getSidewalkBorder() const { return m_sidewalk.getBorder(); }

    //Compute CPU-side bounding boxes for the blocks
    std::vector<BuildingBounds> compute_building_bounds() const;
    std::vector<BuildingBounds> compute_sidewalk_bounds() const;

private:
    wolf::App* m_pApp = nullptr;
    GLuint m_program = 0;
    GLuint m_sidewalkShader = 0;
    GLuint m_roadShader = 0;

    Cube m_cube;
    Sidewalk m_sidewalk;
    District m_district;
    RoadNetwork m_roadNetwork;

    // uniforms cached
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
    float m_minBuildingGap = 2.1f;
    float m_buildingScaleMin = 4.0f;
    float m_buildingScaleMax = 10.0f;
    float m_buildingWidthMin = 1.0f;
    float m_buildingWidthMax = 3.0f;
    float m_buildingDepthMin = 1.0f;
    float m_buildingDepthMax = 3.0f;

    std::vector<CityBlock> m_blocks;
    glm::vec3 m_cityOrigin = glm::vec3(0.0f);
};