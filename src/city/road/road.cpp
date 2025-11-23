#include "road.h"
#include <iostream>

RoadNetwork::RoadNetwork() {}
RoadNetwork::~RoadNetwork() { if (!m_meshes.empty()) cleanup(); }

void RoadNetwork::cleanup() {
    for (auto &m : m_meshes) {
        if (m.vbo) glDeleteBuffers(1, &m.vbo);
        if (m.ebo) glDeleteBuffers(1, &m.ebo);
        if (m.vao) glDeleteVertexArrays(1, &m.vao);
        m = RoadMesh();
    }
    m_meshes.clear();
    m_grid.clear();
    m_program = 0;
    m_uViewProjLoc = -1;
    m_uColorLoc = -1;
}

void RoadNetwork::set_program(GLuint program) {
    m_program = program;
    if (m_program) {
        glUseProgram(m_program);
        m_uViewProjLoc = glGetUniformLocation(m_program, "u_viewProj");
        m_uColorLoc = glGetUniformLocation(m_program, "u_color");
        glUseProgram(0);
    } else {
        m_uViewProjLoc = -1;
        m_uColorLoc = -1;
    }
}

void RoadNetwork::render(const glm::mat4& view_proj) {
    glUseProgram(m_program);
    if (m_uViewProjLoc >= 0) glUniformMatrix4fv(m_uViewProjLoc, 1, GL_FALSE, &view_proj[0][0]);
    if(m_uColorLoc >= 0) glUniform3f(m_uColorLoc, 0.0f, 0.0f, 0.0f);

    for (auto &m : m_meshes) {
        glBindVertexArray(m.vao);
        glDrawElements(GL_TRIANGLES, m.index_count, GL_UNSIGNED_INT, 0);
    }
    glBindVertexArray(0);
    glUseProgram(0);
}

bool RoadNetwork::edge_intersects_building(const glm::ivec2& a, const glm::ivec2& b)
{
    int x0 = a.x;
    int y0 = a.y;
    int x1 = b.x;
    int y1 = b.y;

    int dx = std::abs(x1 - x0);
    int dy = std::abs(y1 - y0);

    int sx = (x0 < x1) ? 1 : -1;
    int sy = (y0 < y1) ? 1 : -1;

    int err = dx - dy;

    int x = x0;
    int y = y0;

    while (true) {

        //Check bounds and building type
        if (in_bounds(x, y))
        {
            if (m_grid[idx(x, y)] == CellType::Building)
                return true;
        }

        //Reached the end of the line
        if (x == x1 && y == y1)
            break;

        int e2 = err * 2;
        if (e2 > -dy) { err -= dy; x += sx; }
        if (e2 <  dx) { err += dx; y += sy; }
    }

    return false;   //No collision
}
