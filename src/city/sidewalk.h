#pragma once
#include "../wolf/wolf.h"
#include "district.h"

struct CityBlock;

class Sidewalk {
public:
    struct SidewalkMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei indexCount = 0;
    };

    Sidewalk();
    ~Sidewalk();

    void setProgram(GLuint program) { m_program = program; }
    GLuint getProgram() const { return m_program; }
    std::vector<SidewalkMesh>& getMeshes() { return m_meshes; }
    float getBorder() const { return m_sidewalkBorder; }

    void createMesh(
        const CityBlock& block,
        const glm::vec3& cityOrigin,
        float blockWidthWorld,
        float blockDepthWorld,
        SidewalkMesh& mesh);

    void renderBlocks();
    void cleanup();

private:
    std::vector<SidewalkMesh> m_meshes;
    GLuint m_program = 0;
    float m_sidewalkBorder = 3.0f;
};