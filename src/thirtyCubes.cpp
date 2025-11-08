#include "thirtyCubes.h"

#ifdef _WIN32
#include <windows.h>
// Windows OpenGL extension loading since glVertexAttribDivisor does not work for this setup
typedef void (WINAPI *PFNGLVERTEXATTRIBDIVISORPROC)(GLuint index, GLuint divisor);
static PFNGLVERTEXATTRIBDIVISORPROC glVertexAttribDivisorPtr = nullptr;

void loadGLExtensions() {
    if (!glVertexAttribDivisorPtr) {
        glVertexAttribDivisorPtr = (PFNGLVERTEXATTRIBDIVISORPROC)wglGetProcAddress("glVertexAttribDivisor");
        if (!glVertexAttribDivisorPtr) {
            printf("WARNING: Could not load glVertexAttribDivisor\n");
        }
    }
}
#endif

struct Vertex
{
    GLfloat x,y,z;
};

static const Vertex gs_squareVertices[] = {
    { -1.0f, -1.0f,  1.0f},
    {  1.0f, -1.0f,  1.0f},
    {  1.0f,  1.0f,  1.0f},
    { -1.0f,  1.0f,  1.0f},


    { -1.0f, -1.0f, -1.0f},
    {  1.0f, -1.0f, -1.0f},
    {  1.0f,  1.0f, -1.0f},
    { -1.0f,  1.0f, -1.0f},
};

static const unsigned int gs_squareIndices[] = {
    0, 1, 2,  2, 3, 0,
    1, 5, 6,  6, 2, 1,
    5, 4, 7,  7, 6, 5,
    4, 0, 3,  3, 7, 4,
    3, 2, 6,  6, 7, 3,
    4, 5, 1,  1, 0, 4
};

ThirtyCubesInstanced::~ThirtyCubesInstanced()
{
    printf("Destroying Instances of Cubes\n");
    glDeleteVertexArrays(1, &m_vao);
    glDeleteBuffers(1, &m_vbo);
    glDeleteBuffers(1, &m_indexBuffer);
    glDeleteProgram(m_program);
    glDeleteBuffers(1, &m_instanceVBO);
    delete m_pOrbitCam;
    delete m_pGrid;
}

void ThirtyCubesInstanced::init() {
    #ifdef _WIN32
    loadGLExtensions();
    #endif
    
    if (!m_program)
    {
        m_program = wolf::LoadShaders("data/cube.vsh", "data/cube.fsh");
        m_pGrid = new Grid3D(50, 1.0f);

        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);

        //Vertex buffer
        glGenBuffers(1, &m_vbo);
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(gs_squareVertices), gs_squareVertices, GL_STATIC_DRAW);

        //Vertex attribute
        int posAttr = glGetAttribLocation(m_program, "a_position");

        glVertexAttribPointer(posAttr, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        glEnableVertexAttribArray(posAttr);

        // Index buffer ---
        glGenBuffers(1, &m_indexBuffer);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_indexBuffer);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(gs_squareIndices), gs_squareIndices, GL_STATIC_DRAW);

        InstanceData instances[30];
        int rows = 10;
        int cols = 3;

        for (int i = 0; i < 30; i++) {
            int row = i / 3;
            int col = i % 3;

            //Handle the spacing later
            instances[i].gridOffset.x = col;
            instances[i].gridOffset.y = row;

            instances[i].position = glm::vec3(0.0f);
            instances[i].rotation = glm::vec3(0.0f);
            instances[i].scale = 1.0f;
        }

        glGenBuffers(1, &m_instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(instances), instances, GL_DYNAMIC_DRAW);

        // Instance attributes
        int gridOffsetAttr = glGetAttribLocation(m_program, "a_gridOffset");
        if (gridOffsetAttr != -1) {
            glVertexAttribPointer(gridOffsetAttr, 2, GL_FLOAT, GL_FALSE, sizeof(InstanceData), 
                                (void*)offsetof(InstanceData, gridOffset));
            glEnableVertexAttribArray(gridOffsetAttr);
        #ifdef _WIN32
            if (glVertexAttribDivisorPtr) glVertexAttribDivisorPtr(gridOffsetAttr, 1);
        #else
            glVertexAttribDivisor(gridOffsetAttr, 1);
        #endif
        }

        int instancePosAttr = glGetAttribLocation(m_program, "a_instancePosition");
        if (instancePosAttr != -1) {
            glVertexAttribPointer(instancePosAttr, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), 
                                (void*)offsetof(InstanceData, position));
            glEnableVertexAttribArray(instancePosAttr);
        #ifdef _WIN32
            if (glVertexAttribDivisorPtr) glVertexAttribDivisorPtr(instancePosAttr, 1);
        #else
            glVertexAttribDivisor(instancePosAttr, 1);
        #endif
        }

        int instanceRotAttr = glGetAttribLocation(m_program, "a_instanceRotation");
        if (instanceRotAttr != -1) {
            glVertexAttribPointer(instanceRotAttr, 3, GL_FLOAT, GL_FALSE, sizeof(InstanceData), 
                                (void*)offsetof(InstanceData, rotation));
            glEnableVertexAttribArray(instanceRotAttr);
        #ifdef _WIN32
            if (glVertexAttribDivisorPtr) glVertexAttribDivisorPtr(instanceRotAttr, 1);
        #else
            glVertexAttribDivisor(instanceRotAttr, 1);
        #endif
        }

        int instanceScaleAttr = glGetAttribLocation(m_program, "a_instanceScale");
        if (instanceScaleAttr != -1) {
            glVertexAttribPointer(instanceScaleAttr, 1, GL_FLOAT, GL_FALSE, sizeof(InstanceData), 
                                (void*)offsetof(InstanceData, scale));
            glEnableVertexAttribArray(instanceScaleAttr);
        #ifdef _WIN32
            if (glVertexAttribDivisorPtr) glVertexAttribDivisorPtr(instanceScaleAttr, 1);
        #else
            glVertexAttribDivisor(instanceScaleAttr, 1);
        #endif
        }

        int instanceColorAttr = glGetAttribLocation(m_program, "a_instanceColor");
        if (instanceColorAttr != -1) {
            glVertexAttribPointer(instanceColorAttr, 4, GL_FLOAT, GL_FALSE, sizeof(InstanceData), 
                                (void*)offsetof(InstanceData, color));
            glEnableVertexAttribArray(instanceColorAttr);
        #ifdef _WIN32
            if (glVertexAttribDivisorPtr) glVertexAttribDivisorPtr(instanceColorAttr, 1);
        #else
            glVertexAttribDivisor(instanceColorAttr, 1);
        #endif
        }

        m_pOrbitCam = new OrbitCamera(m_pApp);
        m_pOrbitCam = new OrbitCamera(m_pApp);
        m_pOrbitCam->focusOn(glm::vec3(-10.0f, -20.0f, -10.0f), glm::vec3(10.0f, 20.0f, 10.0f));
    }
}

void ThirtyCubesInstanced::update(float dt)
{
    m_pOrbitCam->update(dt);
    m_pGrid->update(dt);

    for (int i = 0; i < 30; i++) {
        // Apply rotation speed (degrees per second)
        m_cubes[i].rotation.x += m_cubes[i].rotationSpeed.x * dt;
        m_cubes[i].rotation.y += m_cubes[i].rotationSpeed.y * dt;
        
        // Wrap rotation values to [0, 360) to prevent overflow
        if (m_cubes[i].rotation.x >= 360.0f) m_cubes[i].rotation.x -= 360.0f;
        if (m_cubes[i].rotation.x < 0.0f) m_cubes[i].rotation.x += 360.0f;
        if (m_cubes[i].rotation.y >= 360.0f) m_cubes[i].rotation.y -= 360.0f;
        if (m_cubes[i].rotation.y < 0.0f) m_cubes[i].rotation.y += 360.0f;
    }
}

void ThirtyCubesInstanced::render(int width, int height) {
    glViewport(0, 0, width, height);
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_program || !m_vao || !m_pOrbitCam) {
        return;
    }

    glm::mat4 mProj = m_pOrbitCam->getProjMatrix(width, height);
    glm::mat4 mView = m_pOrbitCam->getViewMatrix();
    glm::mat4 viewProj = mProj * mView;

    if (m_pGrid)
        m_pGrid->render(mView, mProj);

    //Update instance buffer with current cube data from m_cubes array
    InstanceData instances[30];
    float spacing = 7.0f;
    
    for (int i = 0; i < 30; i++) {
        int row = i / 3;
        int col = i % 3;
        
        instances[i].gridOffset.x = (col - 1.0f) * spacing;
        instances[i].gridOffset.y = (row - 4.5f) * spacing;
        instances[i].position = m_cubes[i].position;
        instances[i].rotation = glm::radians(m_cubes[i].rotation);  // Convert to radians
        instances[i].scale = m_cubes[i].scale;
        instances[i].color = m_cubes[i].color;
    } 
    
    //Upload updated instance data
    glBindBuffer(GL_ARRAY_BUFFER, m_instanceVBO);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(instances), instances);

    glUseProgram(m_program);
    glBindVertexArray(m_vao);

    GLint viewProjLoc = glGetUniformLocation(m_program, "u_viewProj");
    glUniformMatrix4fv(viewProjLoc, 1, GL_FALSE, &viewProj[0][0]);

    glDrawElementsInstanced(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0, 30);
}