#include "plane.h"

Plane::Plane() {}

Plane::~Plane() {
    cleanup();
}

void Plane::setProgram(GLuint program) {
    m_program = program;
}

void Plane::createPlaneGeometry(float width, float depth) {
    cleanup();

    // Vertices: positions only (x, y, z)
    std::vector<glm::vec3> vertices = {
        {-0.5f * width, 0.0f, -0.5f * depth}, // 0
        { 0.5f * width, 0.0f, -0.5f * depth}, // 1
        { 0.5f * width, 0.0f,  0.5f * depth}, // 2
        {-0.5f * width, 0.0f,  0.5f * depth}  // 3
    };

    // Indices for two triangles
    std::vector<GLuint> indices = { 0, 1, 2, 2, 3, 0 };
    m_indexCount = static_cast<GLsizei>(indices.size());

    // Create VAO
    glGenVertexArrays(1, &m_vao);
    glBindVertexArray(m_vao);

    // VBO
    glGenBuffers(1, &m_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3), vertices.data(), GL_STATIC_DRAW);

    // Position attribute
    glEnableVertexAttribArray(0); // layout location 0 in shader
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(glm::vec3), (void*)0);

    // EBO
    glGenBuffers(1, &m_ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), indices.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
}

void Plane::bind() {
    glBindVertexArray(m_vao);
}

void Plane::unbind() {
    glBindVertexArray(0);
}

void Plane::cleanup() {
    if (m_vbo) glDeleteBuffers(1, &m_vbo);
    if (m_ebo) glDeleteBuffers(1, &m_ebo);
    if (m_vao) glDeleteVertexArrays(1, &m_vao);

    m_vbo = 0;
    m_ebo = 0;
    m_vao = 0;
}