#include "city_buildings.h"
#include <iostream>

CityBuildings::CityBuildings(wolf::App* pApp, GLuint sharedProgram, GLuint sidewalkShader, GLuint roadShader)
    : m_pApp(pApp), m_program(sharedProgram), m_sidewalkShader(sidewalkShader), m_roadShader(roadShader) {}

CityBuildings::~CityBuildings() {
    m_cube.cleanup();
    m_sidewalk.cleanup();
    m_roadNetwork.cleanup();
    if (m_program) glDeleteProgram(m_program);
    if (m_sidewalkShader) glDeleteProgram(m_sidewalkShader);
    if (m_roadShader) glDeleteProgram(m_roadShader);
}

void CityBuildings::init() {
    m_cube.setProgram(m_program);
    m_cube.createCubeGeometry();

    //Cache uniform locations
    m_uViewProjLoc = glGetUniformLocation(m_program, "u_viewProj");
    m_uSpacingLoc = glGetUniformLocation(m_program, "u_spacing");
    m_uMinHeightLoc = glGetUniformLocation(m_program, "u_buildingScaleMin");
    m_uMaxHeightLoc = glGetUniformLocation(m_program, "u_buildingScaleMax");
    m_uWidthMinLoc = glGetUniformLocation(m_program, "u_buildingWidthMin");
    m_uWidthMaxLoc = glGetUniformLocation(m_program, "u_buildingWidthMax");
    m_uDepthMinLoc = glGetUniformLocation(m_program, "u_buildingDepthMin");
    m_uDepthMaxLoc = glGetUniformLocation(m_program, "u_buildingDepthMax");
    m_uBlockOffsetLoc = glGetUniformLocation(m_program, "u_blockOffset");
    m_uGridWLoc = glGetUniformLocation(m_program, "u_gridW");
    m_uMinBuildingGapLoc = glGetUniformLocation(m_program, "u_minBuildingGap");

    m_roadNetwork.set_program(m_roadShader);
}

void CityBuildings::update(float dt) {
}

void CityBuildings::render(const glm::mat4& viewProj) {
    //Render sidewalks
    if (m_sidewalkShader) {
        glUseProgram(m_sidewalkShader);
        GLint uViewProjLoc = glGetUniformLocation(m_sidewalkShader, "u_viewProj");
        GLint uColorLoc = glGetUniformLocation(m_sidewalkShader, "u_color");
        if (uViewProjLoc >= 0) glUniformMatrix4fv(uViewProjLoc, 1, GL_FALSE, &viewProj[0][0]);
        if (uColorLoc >= 0) glUniform4f(uColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
        m_sidewalk.renderBlocks();
    }

    //Render roads
    if (m_roadShader) {
        glUseProgram(m_roadShader);
    }
    m_roadNetwork.render(viewProj);

    //Render buildings
    glUseProgram(m_program);
    glUniformMatrix4fv(m_uViewProjLoc, 1, GL_FALSE, &viewProj[0][0]);
    m_cube.bind();
    for (auto& block : m_blocks) {
        glm::vec3 blockPos = block.origin + m_cityOrigin;
        glUniform3fv(m_uBlockOffsetLoc, 1, &blockPos[0]);
        glUniform1f(m_uSpacingLoc, m_minBuildingGap);
        glUniform1f(m_uMinHeightLoc, m_buildingScaleMin);
        glUniform1f(m_uMaxHeightLoc, m_buildingScaleMax);
        glUniform1f(m_uWidthMinLoc, m_buildingWidthMin);
        glUniform1f(m_uWidthMaxLoc, m_buildingWidthMax);
        glUniform1f(m_uDepthMinLoc, m_buildingDepthMin);
        glUniform1f(m_uDepthMaxLoc, m_buildingDepthMax);
        glUniform1f(m_uGridWLoc, float(block.width));
        glUniform1f(m_uMinBuildingGapLoc, m_minBuildingGap);

        int instanceCount = block.width * block.depth;
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, instanceCount);
    }
    m_cube.unbind();
    glUseProgram(0);
}

std::vector<CityBlock> CityBuildings::generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize) {
    m_blocks = m_district.generateRandomCity(
        minGridSize, maxGridSize, minBlockSize, maxBlockSize,
        m_buildingWidthMax, m_buildingDepthMax, m_minBuildingGap, m_cityOrigin);

    m_sidewalk.cleanup();
    m_sidewalk.setProgram(m_sidewalkShader);

    for (auto& block : m_blocks) {
        float blockWidthWorld  = block.width  * m_buildingWidthMax  + (block.width  - 1) * m_minBuildingGap;
        float blockDepthWorld  = block.depth  * m_buildingDepthMax  + (block.depth  - 1) * m_minBuildingGap;

        Sidewalk::SidewalkMesh mesh;
        m_sidewalk.createMesh(block, m_cityOrigin, blockWidthWorld, blockDepthWorld, mesh);
        m_sidewalk.getMeshes().push_back(mesh);
    }

    //Compute bounds and pass them to road network
    auto bounds = compute_sidewalk_bounds();

    //Grid parameters
    float cell_size = std::max(m_buildingWidthMax, m_buildingDepthMax) * 0.99f; //Cells slightly smaller than building footprint
    int grid_w = 512;
    int grid_h = 512;

    //Build roads (A* connecting seeds drawn from block centers)
    std::vector<glm::vec3> seeds;
    for (auto &b : bounds) {
        glm::vec3 center = (b.min + b.max) * 0.5f;
        seeds.push_back(center);
    }
    //Let road_network auto-generate a few edge seeds too if seeds are empty but feed seeds above
    m_roadNetwork.build_from_buildings(bounds, m_cityOrigin, cell_size, grid_w, grid_h, seeds);
    m_roadNetwork.set_program(m_roadShader);

    return m_blocks;
}

std::vector<BuildingBounds> CityBuildings::compute_building_bounds() const {
    std::vector<BuildingBounds> bounds;
    bounds.reserve(m_blocks.size());
    for (auto &block : m_blocks) {
        float widthWorld = block.width  * m_buildingWidthMax  + (block.width  - 1) * m_minBuildingGap;
        float depthWorld = block.depth  * m_buildingDepthMax  + (block.depth  - 1) * m_minBuildingGap;
        glm::vec3 min(block.origin.x + m_cityOrigin.x, 0.0f, block.origin.z + m_cityOrigin.z);
        glm::vec3 max(min.x + widthWorld, 0.0f, min.z + depthWorld);
        bounds.push_back({min, max});
    }
    return bounds;
}

std::vector<BuildingBounds> CityBuildings::compute_sidewalk_bounds() const {
    float border = getSidewalkBorder();

    std::vector<BuildingBounds> sidewalkBounds;
    sidewalkBounds.reserve(m_blocks.size());

    for (auto &block : m_blocks) {
        float blockWidthWorld = block.width  * m_buildingWidthMax
                              + (block.width - 1) * m_minBuildingGap;

        float blockDepthWorld = block.depth  * m_buildingDepthMax
                              + (block.depth - 1) * m_minBuildingGap;

        glm::vec3 min(block.origin.x + m_cityOrigin.x - border,
                      0.0f,
                      block.origin.z + m_cityOrigin.z - border);

        glm::vec3 max(block.origin.x + m_cityOrigin.x + blockWidthWorld + border,
                      0.0f,
                      block.origin.z + m_cityOrigin.z + blockDepthWorld + border);

        sidewalkBounds.push_back({min, max});
    }
    return sidewalkBounds;
}