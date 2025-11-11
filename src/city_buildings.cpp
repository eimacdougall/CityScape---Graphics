#include "city_buildings.h"

CityBuildings::CityBuildings(wolf::App* pApp, GLuint sharedProgram)
    : m_pApp(pApp), m_program(sharedProgram) {}

CityBuildings::~CityBuildings() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteVertexArrays(1, &m_vao);
    
    if (!m_program) //Only delete if this instance owns the program (shouldn't matter rn)
        glDeleteProgram(m_program);

    delete m_pOrbitCam;
}

void CityBuildings::init() {
    //Make Flycam an have that in main
    m_pOrbitCam = new OrbitCamera(m_pApp);
    m_pOrbitCam->focusOn(glm::vec3(-50.0f, -20.0f, -50.0f),
                          glm::vec3(50.0f, 20.0f, 50.0f));

    createCubeGeometry();

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

//Handles the creation of cubes before they become buildings
void CityBuildings::createCubeGeometry() {
    GLfloat vertices[] = {
        -0.5f,-0.5f,-0.5f,  0.5f,-0.5f,-0.5f,  0.5f,0.5f,-0.5f,  -0.5f,0.5f,-0.5f,
        -0.5f,-0.5f, 0.5f,  0.5f,-0.5f, 0.5f,  0.5f,0.5f, 0.5f,  -0.5f,0.5f, 0.5f
    };

    GLuint indices[] = {
        0,1,2, 2,3,0,
        4,5,6, 6,7,4,
        0,1,5, 5,4,0,
        6,7,3, 3,2,6,
        1,2,6, 6,5,1,
        0,3,7, 7,4,0
    };

    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &m_indexBuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLint posLoc = glGetAttribLocation(m_program, "a_position");
    if (posLoc != -1) {
        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    }

    glBindVertexArray(0);
}

void CityBuildings::update(float dt) {
    if (m_pOrbitCam) m_pOrbitCam->update(dt);
}

void CityBuildings::render(int width, int height) {
    if (!m_program || !m_pOrbitCam) return;

    glUseProgram(m_program);

    glm::mat4 viewProj = m_pOrbitCam->getProjMatrix(width, height) * m_pOrbitCam->getViewMatrix();
    glUniformMatrix4fv(m_uViewProjLoc, 1, GL_FALSE, &viewProj[0][0]);

    glBindVertexArray(m_vao);

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

    glBindVertexArray(0);
    glUseProgram(0);
}

void CityBuildings::removeLastBlock() {
    if (!m_blocks.empty()) m_blocks.pop_back();
}

void CityBuildings::clearBlocks() {
    m_blocks.clear();
}

void CityBuildings::generateRandomCity(int minGridSize, int maxGridSize, int minBlockSize, int maxBlockSize) {
    //Clear the city
    m_blocks.clear();

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

            //Calculate origin so blocks are spaced by their size + gap
            float blockSpacingX = blockWidth * m_buildingWidthMax + m_minBuildingGap * (blockWidth + 1);
            float blockSpacingZ = blockDepth * m_buildingDepthMax + m_minBuildingGap * (blockDepth + 1);

            glm::vec3 origin(
                x * blockSpacingX,
                0.0f,
                z * blockSpacingZ
            );

            //Add city origin offset
            origin += m_cityOrigin;


            m_blocks.push_back({origin, blockWidth, blockDepth});
        }
    }
}