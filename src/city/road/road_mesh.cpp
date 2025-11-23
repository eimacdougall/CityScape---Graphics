#include "road.h"

void RoadNetwork::rasterize_paths_to_roads(const std::vector<std::vector<glm::ivec2>>& paths) {
    for (auto &path : paths) {
    for (size_t i = 1; i < path.size(); ++i) {
        auto a = path[i-1];
        auto b = path[i];
        //If horizontal or vertical gap > 1 cell, fill intermediate cells
        int dx = b.x - a.x;
        int dy = b.y - a.y;
        int steps = std::max(std::abs(dx), std::abs(dy));
        for (int s = 0; s <= steps; ++s) {
            int x = a.x + (dx * s) / steps;
            int y = a.y + (dy * s) / steps;
            if (m_grid[idx(x, y)] == CellType::Empty)
                m_grid[idx(x, y)] = CellType::Road;
        }
    }
}
}

void RoadNetwork::create_meshes_from_roads(float road_width) {
    std::vector<glm::vec3> verts;
    std::vector<GLuint> inds;
    verts.reserve(1024); inds.reserve(1024);

    for (int y=0; y<m_gridH; ++y) {
        for (int x=0; x<m_gridW; ++x) {
            if (m_grid[idx(x,y)] != CellType::Road) continue;
            glm::vec3 centerA = m_cityOrigin + glm::vec3(x*m_cellSize + m_cellSize*0.5f, 0.0f, y*m_cellSize + m_cellSize*0.5f);

            int nx = x+1, ny = y;
            if (in_bounds(nx,ny) && m_grid[idx(nx,ny)]==CellType::Road) {
                glm::vec3 centerB = m_cityOrigin + glm::vec3(nx*m_cellSize + m_cellSize*0.5f,0.0f,ny*m_cellSize + m_cellSize*0.5f);
                push_road_segment_geom(verts, inds, centerA, centerB, road_width);
            }
            nx = x; ny = y+1;
            if (in_bounds(nx,ny) && m_grid[idx(nx,ny)]==CellType::Road) {
                glm::vec3 centerB = m_cityOrigin + glm::vec3(nx*m_cellSize + m_cellSize*0.5f,0.0f,ny*m_cellSize + m_cellSize*0.5f);
                push_road_segment_geom(verts, inds, centerA, centerB, road_width);
            }
        }
    }

    if (verts.empty() || inds.empty()) return;

    RoadMesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size()*sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(void*)0);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size()*sizeof(GLuint), inds.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);
    mesh.index_count = static_cast<GLsizei>(inds.size());
    m_meshes.push_back(mesh);
}

void RoadNetwork::push_road_segment_geom(std::vector<glm::vec3>& verts, std::vector<GLuint>& inds, const glm::vec3& a, const glm::vec3& b, float width) {
    glm::vec3 dir = b - a;
    dir.y = 0.0f;
    float len = glm::length(dir);
    if (len <= 1e-6f) return;
    dir /= len;
    glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0.0f,1.0f,0.0f))) * (width*0.5f);

    GLuint base = static_cast<GLuint>(verts.size());
    verts.push_back(a+right);
    verts.push_back(a-right);
    verts.push_back(b+right);
    verts.push_back(b-right);

    inds.push_back(base+0); inds.push_back(base+1); inds.push_back(base+2);
    inds.push_back(base+2); inds.push_back(base+1); inds.push_back(base+3);
}