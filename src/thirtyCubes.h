#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/OrbitCamera.h"
#include "../samplefw/Grid3D.h"

struct CubeInstance {
    glm::vec3 position;
    glm::vec3 rotation;  // x, y, z rotation in degrees
    glm::vec4 color;
    float scale;
    glm::vec2 rotationSpeed;
};

struct InstanceData {
    glm::vec2 gridOffset;
    glm::vec3 position;
    glm::vec3 rotation;
    glm::vec4 color;
    float scale;
};

class ThirtyCubesInstanced: public Sample
{
public:
    ThirtyCubesInstanced(wolf::App* pApp) : Sample(pApp,"Interleaved with Index Buffers") {
        //Initialize all 30 cubes with default values
        for (int i = 0; i < 30; i++) {
            m_cubes[i].position = glm::vec3(0.0f);
            m_cubes[i].rotation = glm::vec3(0.0f);
            m_cubes[i].scale = 1.0f;
            m_cubes[i].color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            m_cubes[i].rotationSpeed = glm::vec2(0.0f, 0.0f);
        }
    }
    ~ThirtyCubesInstanced();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;
    
    //Individual cube control
    void setCubeTransform(int index, const glm::vec3& rotation, float scale, const glm::vec3& position) {
        if (index >= 0 && index < 30) {
            m_cubes[index].rotation = rotation;
            m_cubes[index].scale = scale;
            m_cubes[index].position = position;
        }
    }
    
    CubeInstance& getCube(int index) {
        static CubeInstance dummy;
        if (index >= 0 && index < 30) return m_cubes[index];
        return dummy;
    }
    
    int getCubeCount() const { return 30; }

private:
    GLuint m_program = 0;
    GLuint m_vbo = 0;
    GLuint m_indexBuffer = 0;
    GLuint m_vao = 0;
    GLuint m_instanceVBO = 0;

    OrbitCamera* m_pOrbitCam = nullptr;
    Grid3D* m_pGrid = nullptr;

    CubeInstance m_cubes[30];
};