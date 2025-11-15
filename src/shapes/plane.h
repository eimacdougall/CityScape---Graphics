#pragma once
#include "../wolf/wolf.h"

class Plane {
public:
    Plane();
    ~Plane();

    void setProgram(GLuint program);       // Shader program
    void createPlaneGeometry(float width = 1.0f, float depth = 1.0f); // Plane size
    void bind();
    void unbind();
    void cleanup();

private:
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_ebo = 0;
    GLuint m_program = 0;
    GLsizei m_indexCount = 0;
};