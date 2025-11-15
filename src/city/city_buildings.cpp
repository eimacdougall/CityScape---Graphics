#include "city_buildings.h"

CityBuildings::CityBuildings(wolf::App* pApp, GLuint sharedProgram, GLuint sidewalkShader)
    : m_pApp(pApp), m_program(sharedProgram), m_sidewalkShader(sidewalkShader) {}

CityBuildings::~CityBuildings() {
    m_cube.cleanup();
    m_sidewalk.cleanup();
    if (m_program) glDeleteProgram(m_program);
    if (m_sidewalkShader) glDeleteProgram(m_sidewalkShader);
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
}

void CityBuildings::update(float dt) {
}

void CityBuildings::render(const glm::mat4& viewProj) {
    //Render sidewalks
    glUseProgram(m_sidewalkShader);
    GLint uViewProjLoc = glGetUniformLocation(m_sidewalkShader, "u_viewProj");
    GLint uColorLoc = glGetUniformLocation(m_sidewalkShader, "u_color");
    glUniformMatrix4fv(uViewProjLoc, 1, GL_FALSE, &viewProj[0][0]);
    glUniform4f(uColorLoc, 0.5f, 0.5f, 0.5f, 1.0f);
    m_sidewalk.renderBlocks();

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


void CityBuildings::removeLastBlock() {
    if (!m_blocks.empty()) m_blocks.pop_back();
}

void CityBuildings::clearBlocks() {
    m_blocks.clear();
}

void CityBuildings::generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize) {
    m_blocks = m_district.generateRandomCity(
        minGridSize, maxGridSize, minBlockSize, maxBlockSize,
        m_buildingWidthMax, m_buildingDepthMax, m_minBuildingGap, m_cityOrigin);

    m_sidewalk.cleanup();
    m_sidewalk.setProgram(m_sidewalkShader);

    for (auto& block : m_blocks) {
        float blockWidthWorld  = block.width  * m_buildingWidthMax  + (block.width  - 1) * m_minBuildingGap;
        float blockDepthWorld  = block.depth  * m_buildingDepthMax  + (block.depth  - 1) * m_minBuildingGap;

        Sidewalk::SidewalkMesh mesh;
        m_sidewalk.createMesh(block, blockWidthWorld, blockDepthWorld, 3.0f, mesh);
        m_sidewalk.getMeshes().push_back(mesh);
    }
}