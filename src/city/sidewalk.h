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

    void createMesh(
        const CityBlock& block,
        float blockWidthWorld,
        float blockDepthWorld,
        float border,
        SidewalkMesh& mesh);

    void renderBlocks();
    void cleanup();

private:
    std::vector<SidewalkMesh> m_meshes;
    GLuint m_program = 0;
};