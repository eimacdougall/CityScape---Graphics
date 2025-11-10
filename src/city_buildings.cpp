#include "city_buildings.h"

CityBuildings::CityBuildings(wolf::App* pApp)
    : Sample(pApp, "Procedural City") {}

CityBuildings::~CityBuildings() {
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteVertexArrays(1, &m_vao);
    glDeleteProgram(m_program);
    delete m_pOrbitCam;
}

void CityBuildings::init() {
    m_pOrbitCam = new OrbitCamera(m_pApp);
    m_pOrbitCam->focusOn(glm::vec3(-50.0f, -20.0f, -50.0f), glm::vec3(50.0f, 20.0f, 50.0f));

    m_program = wolf::LoadShaders("data/city_buildings.vsh", "data/city_buildings.fsh");
    createCubeGeometry();

    //Cache uniform locations
    m_uViewProjLoc   = glGetUniformLocation(m_program, "u_viewProj");
    m_uSpacingLoc    = glGetUniformLocation(m_program, "u_spacing");
    m_uMinHeightLoc  = glGetUniformLocation(m_program, "u_buildingScaleMin");
    m_uMaxHeightLoc  = glGetUniformLocation(m_program, "u_buildingScaleMax");
    m_uWidthMinLoc   = glGetUniformLocation(m_program, "u_buildingWidthMin");
    m_uWidthMaxLoc   = glGetUniformLocation(m_program, "u_buildingWidthMax");
    m_uDepthMinLoc   = glGetUniformLocation(m_program, "u_buildingDepthMin");
    m_uDepthMaxLoc   = glGetUniformLocation(m_program, "u_buildingDepthMax");
    m_uBlockOffsetLoc= glGetUniformLocation(m_program, "u_blockOffset");
    m_uGridWLoc      = glGetUniformLocation(m_program, "u_gridW");
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
    if(posLoc != -1) {
        glEnableVertexAttribArray(posLoc);
        glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), 0);
    }

    glBindVertexArray(0);
}

void CityBuildings::update(float dt) {
    if (m_pOrbitCam) m_pOrbitCam->update(dt);
}

void CityBuildings::render(int width, int height) {
    if(!m_program || !m_pOrbitCam) return;

    glViewport(0,0,width,height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.15f,0.15f,0.15f,1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);

    glm::mat4 viewProj = m_pOrbitCam->getProjMatrix(width,height) * m_pOrbitCam->getViewMatrix();
    glUniformMatrix4fv(m_uViewProjLoc,1,GL_FALSE,&viewProj[0][0]);

    glBindVertexArray(m_vao);

    for (auto& block : m_blocks) {
        glUniform3fv(m_uBlockOffsetLoc, 1, &block.origin[0]);
        glUniform1f(m_uSpacingLoc, 1.0f);
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

glm::vec3 CityBuildings::computeBlockOffset(int blockIndex, int blockWidth, int blockDepth) const {
    int totalBlocks = m_blocks.size() + 1;
    int gridSize = (int)ceil(sqrt((float)totalBlocks));

    int row = blockIndex / gridSize;
    int col = blockIndex % gridSize;

    float x = 0.0f;
    for (int i = row * gridSize; i < row * gridSize + col; i++) {
        if (i < m_blocks.size())
            x += m_blocks[i].width * m_blockSpacing;
        else
            x += blockWidth * m_blockSpacing;
    }

    float z = 0.0f;
    for (int r = 0; r < row; r++) {
        int start = r * gridSize;
        int end = std::min(start + gridSize, (int)m_blocks.size());
        float maxDepth = 0.0f;
        for (int i = start; i < end; i++) {
            maxDepth = std::max(maxDepth, (float)m_blocks[i].depth);
        }
        z += maxDepth * m_blockSpacing;
    }

    return glm::vec3(x, 0.0f, z);
}

void CityBuildings::addBlockWithGridPlacement(int blockWidth, int blockDepth) {
    glm::vec3 offset = computeBlockOffset(m_blocks.size(), blockWidth, blockDepth);
    addBlock(offset, blockWidth, blockDepth);
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
            
            // Calculate block origin based on grid position and block size
            glm::vec3 origin(
                x * (blockWidth + m_blockSpacing),
                0.0f,
                z * (blockDepth + m_blockSpacing)
            );

            m_blocks.push_back({origin, blockWidth, blockDepth});
        }
    }
}
