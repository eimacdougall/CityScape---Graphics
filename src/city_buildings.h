#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/OrbitCamera.h"
#include <random> 

struct CityBlock {
    glm::vec3 origin;   // world-space position
    int width;          // buildings along X
    int depth;          // buildings along Z
};

class CityBuildings : public Sample {
public:
    CityBuildings(wolf::App* pApp);
    ~CityBuildings();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;

    GLuint getProgram() const { return m_program; }

    // Setters
    void setSpacing(float s) { m_blockSpacing = s; }
    void setBuildingHeightRange(float minH, float maxH) { m_buildingScaleMin = minH; m_buildingScaleMax = maxH; }
    void setBuildingSizeRange(float minW, float maxW, float minD, float maxD) {
        m_buildingWidthMin = minW; m_buildingWidthMax = maxW;
        m_buildingDepthMin = minD; m_buildingDepthMax = maxD;
    }

    void addBlock(const glm::vec3& origin, int width, int depth) { 
        m_blocks.push_back({origin, width, depth}); 
    }
    void removeLastBlock();
    void clearBlocks();

    //Grid-aware placement
    void addBlockWithGridPlacement(int blockWidth, int blockDepth);
    glm::vec3 computeBlockOffset(int blockIndex, int blockWidth, int blockDepth) const;

    //Full city generation
    void generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize);

private:
    void createCubeGeometry();

    GLuint m_program = 0;
    GLuint m_vbo = 0;
    GLuint m_indexBuffer = 0;
    GLuint m_vao = 0;

    OrbitCamera* m_pOrbitCam = nullptr;

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

    //Parameters
    float m_blockSpacing = 10.0f; //Distance between blocks
    float m_buildingScaleMin = 2.0f;
    float m_buildingScaleMax = 12.0f;
    float m_buildingWidthMin = 0.8f;
    float m_buildingWidthMax = 1.2f;
    float m_buildingDepthMin = 0.8f;
    float m_buildingDepthMax = 1.2f;

    std::vector<CityBlock> m_blocks;
};