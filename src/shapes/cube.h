#pragma once
#include "../wolf/wolf.h"

class Cube {
public:
    Cube();
    ~Cube();

    void setProgram(GLuint program);

    // Handles the creation of cubes before they become buildings
    void createCubeGeometry();

    void bind() const;
    void unbind() const;
    void cleanup();

private:
    GLuint m_program = 0; // Can be shared across instances
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_indexBuffer = 0;
};