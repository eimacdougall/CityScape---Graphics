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
    const glm::vec3& cityOrigin,
    float blockWidthWorld,
    float blockDepthWorld,
    SidewalkMesh& mesh)
{
    float border = m_sidewalkBorder;
    float yTop = 0.0f;
    float curbHeight = 0.2f;
    float yBottom = yTop - curbHeight;

    float shortenAmount = 2.1f; //To prevent sidewalk being to close or far from buildings

    //Center the sidewalk around the corner block and make sure to base off city origin
    float worldX = cityOrigin.x + block.origin.x;
    float worldZ = cityOrigin.z + block.origin.z;

    float centerX = worldX + blockWidthWorld * 0.5f;
    float centerZ = worldZ + blockDepthWorld * 0.5f;

    float halfWidth  = blockWidthWorld * 0.5f + border;
    float halfDepth  = blockDepthWorld * 0.5f + border;

    float outerLeft   = centerX - halfWidth - (shortenAmount/2.0f);
    float outerRight  = centerX + halfWidth - shortenAmount;
    float outerFront  = centerZ - halfDepth;
    float outerBack   = centerZ + halfDepth - shortenAmount;

    float innerLeft   = centerX - blockWidthWorld * 0.5f - (shortenAmount/2.0f);
    float innerRight  = centerX + blockWidthWorld * 0.5f - shortenAmount;
    float innerFront  = centerZ - blockDepthWorld * 0.5f;
    float innerBack   = centerZ + blockDepthWorld * 0.5f - shortenAmount;

    std::vector<glm::vec3> vertices = {
        //Top outer (0–3)
        {outerLeft,  yTop, outerFront},
        {outerRight, yTop, outerFront},
        {outerRight, yTop, outerBack},
        {outerLeft,  yTop, outerBack},

        //Top inner (4–7)
        {innerLeft,  yTop, innerFront},
        {innerRight, yTop, innerFront},
        {innerRight, yTop, innerBack},
        {innerLeft,  yTop, innerBack},

        //Bottom outer (8–11)
        {outerLeft,  yBottom, outerFront},
        {outerRight, yBottom, outerFront},
        {outerRight, yBottom, outerBack},
        {outerLeft,  yBottom, outerBack},

        //Bottom inner (12–15)
        {innerLeft,  yBottom, innerFront},
        {innerRight, yBottom, innerFront},
        {innerRight, yBottom, innerBack},
        {innerLeft,  yBottom, innerBack}
    };

    std::vector<GLuint> indices = {
        //Top ring
        0,1,5,  0,5,4,
        1,2,6,  1,6,5,
        2,3,7,  2,7,6,
        3,0,4,  3,4,7,

        //Outer vertical walls
        0,1,9,  0,9,8,
        1,2,10, 1,10,9,
        2,3,11, 2,11,10,
        3,0,8,  3,8,11,

        //Inner vertical walls
        4,5,13, 4,13,12,
        5,6,14, 5,14,13,
        6,7,15, 6,15,14,
        7,4,12, 7,12,15
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