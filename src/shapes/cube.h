#pragma once
#include "../wolf/wolf.h"

class Cube {
public:
    Cube() = default;
    ~Cube() = default;

    void setProgram(GLuint program) { m_program = program; }

    //Handles the creation of cubes before they become buildings
    void createCubeGeometry() {
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

    void bind() const { glBindVertexArray(m_vao); }
    void unbind() const { glBindVertexArray(0); }

    void cleanup() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteBuffers(1, &m_indexBuffer);
        glDeleteVertexArrays(1, &m_vao);
    }
    
private:
    GLuint m_program = 0; // Can be shared across instances
    GLuint m_vao = 0;
    GLuint m_vbo = 0;
    GLuint m_indexBuffer = 0;
};