#include "sidewalk.h"

Sidewalk::Sidewalk() {}
Sidewalk::~Sidewalk() { cleanup(); }

void Sidewalk::cleanup() {
    for (auto& mesh : m_meshes) {
        if (mesh.vbo) glDeleteBuffers(1, &mesh.vbo);
        if (mesh.ebo) glDeleteBuffers(1, &mesh.ebo);
        if (mesh.vao) glDeleteVertexArrays(1, &mesh.vao);
        mesh = SidewalkMesh();
    }
    m_meshes.clear();
}

void Sidewalk::createMesh(
    const CityBlock& block,
    float blockWidthWorld,
    float blockDepthWorld,
    float border,
    SidewalkMesh& mesh)
{
    float yOffset = 0.0f;
    float shortenAmount = 2.0f; //To prevent sidewalk being to close or far from buildings

    //Center the sidewalk around the corner block
    float centerX = block.origin.x + blockWidthWorld * 0.5f;
    float centerZ = block.origin.z + blockDepthWorld * 0.5f;

    float halfWidth  = blockWidthWorld * 0.5f + border;
    float halfDepth  = blockDepthWorld * 0.5f + border;

    float outerLeft   = centerX - halfWidth;
    float outerRight  = centerX + halfWidth;
    float outerFront  = centerZ - halfDepth;
    float outerBack   = centerZ + halfDepth - shortenAmount;

    float innerLeft   = centerX - blockWidthWorld * 0.5f;
    float innerRight  = centerX + blockWidthWorld * 0.5f;
    float innerFront  = centerZ - blockDepthWorld * 0.5f;
    float innerBack   = centerZ + blockDepthWorld * 0.5f - shortenAmount;

    std::vector<glm::vec3> vertices = {
        {outerLeft, yOffset, outerFront},
        {outerRight, yOffset, outerFront},
        {outerRight, yOffset, outerBack},
        {outerLeft, yOffset, outerBack},
        {innerLeft, yOffset, innerFront},
        {innerRight, yOffset, innerFront},
        {innerRight, yOffset, innerBack},
        {innerLeft, yOffset, innerBack}
    };

    std::vector<GLuint> indices = {
        0,1,5, 0,5,4,
        1,2,6, 1,6,5,
        2,3,7, 2,7,6,
        3,0,4, 3,4,7
    };

    mesh.indexCount = static_cast<GLsizei>(indices.size());

    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Sidewalk::renderBlocks() {
    if (!m_program) return;

    glUseProgram(m_program);
    for (auto& mesh : m_meshes) {
        glBindVertexArray(mesh.vao);
        glDrawElements(GL_TRIANGLES, mesh.indexCount, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
}