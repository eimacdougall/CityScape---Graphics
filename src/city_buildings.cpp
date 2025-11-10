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
    m_pOrbitCam->focusOn(glm::vec3(-10.0f, -20.0f, -10.0f),
                          glm::vec3(10.0f, 20.0f, 10.0f));

    m_program = wolf::LoadShaders("data/city_buildings.vsh", "data/city_buildings.fsh");
    createCubeGeometry();

    // Cache uniform locations
    m_uViewProjLoc   = glGetUniformLocation(m_program, "u_viewProj");
    m_uSpacingLoc    = glGetUniformLocation(m_program, "u_spacing");
    m_uMinHeightLoc  = glGetUniformLocation(m_program, "u_buildingScaleMin");
    m_uMaxHeightLoc  = glGetUniformLocation(m_program, "u_buildingScaleMax");
}

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
    if (m_pOrbitCam)
        m_pOrbitCam->update(dt);
}

void CityBuildings::render(int width, int height) {
    if (!m_program || !m_pOrbitCam)
        return;
    
    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(m_program);
    glm::mat4 viewProj = m_pOrbitCam->getProjMatrix(width, height) * m_pOrbitCam->getViewMatrix();
    glUniformMatrix4fv(m_uViewProjLoc, 1, GL_FALSE, &viewProj[0][0]);
    glUniform1f(m_uSpacingLoc, m_spacing);
    glUniform1f(m_uMinHeightLoc, m_buildingScaleMin);
    glUniform1f(m_uMaxHeightLoc, m_buildingScaleMax);

    glBindVertexArray(m_vao);

    for (auto& block : m_blocks) {
        // Send per-block uniforms to shader
        glUniform3f(glGetUniformLocation(m_program, "u_blockOrigin"), 
                    block.origin.x, block.origin.y, block.origin.z);
        glUniform2i(glGetUniformLocation(m_program, "u_blockSize"),
                    block.width, block.depth);

        // Each block has width*depth instances
        glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, block.width * block.depth);
    }

    glBindVertexArray(0);
    glUseProgram(0);
}