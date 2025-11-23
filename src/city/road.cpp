#include "road.h"
#include <iostream>

RoadNetwork::RoadNetwork() {}
RoadNetwork::~RoadNetwork() { 
    if (!m_meshes.empty()) cleanup(); 
}

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

void RoadNetwork::build_from_buildings(
    const std::vector<BuildingBounds>& bounds,
    const glm::vec3& city_origin,
    float cell_size,
    int grid_w,
    int grid_h,
    const std::vector<glm::vec3>& seeds
) {
    cleanup();
    m_cityOrigin = city_origin;
    m_cellSize = cell_size;
    m_gridW = grid_w;
    m_gridH = grid_h;
    m_grid.assign(grid_w * grid_h, CellType::Empty);

    // Mark buildings as blocked
    mark_buildings_as_blocked(bounds);

    std::vector<glm::ivec2> seed_cells;
    mark_seed_cells(seeds, seed_cells);

    // Add corner and mid-edge seeds per block
    addSeedsFromBlocks(bounds);
    seed_cells.insert(seed_cells.end(), m_extraSeeds.begin(), m_extraSeeds.end());

    if (seed_cells.size() < 2) return;

    // --- MST connection: always connect nearest unconnected seed ---
    std::vector<std::vector<glm::ivec2>> all_paths;
    std::vector<bool> connected(seed_cells.size(), false);
    connected[0] = true;
    std::vector<int> connected_ids = {0};

    while (connected_ids.size() < seed_cells.size()) {
        float best_dist = std::numeric_limits<float>::infinity();
        int best_from = -1, best_to = -1;

        // Find nearest pair (connected -> unconnected)
        for (int c_idx : connected_ids) {
            glm::ivec2 c = seed_cells[c_idx];
            for (size_t i = 0; i < seed_cells.size(); ++i) {
                if (connected[i]) continue;
                glm::ivec2 u = seed_cells[i];
                float d = glm::distance(glm::vec2(c.x, c.y), glm::vec2(u.x, u.y));
                if (d < best_dist) {
                    best_dist = d;
                    best_from = c_idx;
                    best_to = (int)i;
                }
            }
        }

        if (best_from == -1 || best_to == -1) break;

        // Connect best pair
        auto path = find_path_astar(seed_cells[best_from], seed_cells[best_to]);
        if (!path.empty()) all_paths.push_back(path);

        connected[best_to] = true;
        connected_ids.push_back(best_to);
    }

    rasterize_paths_to_roads(all_paths);
    create_meshes_from_roads(3.5f);
}
/*void RoadNetwork::addSeedsFromBlocks(const std::vector<BuildingBounds>& bounds) {
    m_extraSeeds.clear();

    for (size_t i = 0; i < bounds.size(); ++i) {
        auto &b = bounds[i];

        // Corners
        glm::vec3 corners[4] = {
            {b.min.x, 0.0f, b.min.z},
            {b.max.x, 0.0f, b.min.z},
            {b.max.x, 0.0f, b.max.z},
            {b.min.x, 0.0f, b.max.z}
        };
        for (auto &c : corners) {
            int cx = int((c.x - m_cityOrigin.x) / m_cellSize + 0.5f);
            int cz = int((c.z - m_cityOrigin.z) / m_cellSize + 0.5f);
            if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                m_extraSeeds.push_back(glm::ivec2(cx, cz));
        }

        // Mid-edges
        glm::vec3 mid_edges[4] = {
            {(b.min.x + b.max.x) * 0.5f, 0.0f, b.min.z}, // top
            {(b.min.x + b.max.x) * 0.5f, 0.0f, b.max.z}, // bottom
            {b.min.x, 0.0f, (b.min.z + b.max.z) * 0.5f}, // left
            {b.max.x, 0.0f, (b.min.z + b.max.z) * 0.5f}  // right
        };
        for (auto &m : mid_edges) {
            int cx = int((m.x - m_cityOrigin.x) / m_cellSize + 0.5f);
            int cz = int((m.z - m_cityOrigin.z) / m_cellSize + 0.5f);
            if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                m_extraSeeds.push_back(glm::ivec2(cx, cz));
        }

        // --- Diagonal / neighbor connections ---
        for (size_t j = 0; j < bounds.size(); ++j) {
            if (i == j) continue;
            auto &n = bounds[j];

            // Check if neighboring block (touching sides)
            bool neighborX = (b.max.x >= n.min.x - m_cellSize && b.min.x <= n.max.x + m_cellSize);
            bool neighborZ = (b.max.z >= n.min.z - m_cellSize && b.min.z <= n.max.z + m_cellSize);

            if (neighborX) {
                // Horizontal neighbor: add mid-edge vertical seeds
                float midZ = (std::max(b.min.z, n.min.z) + std::min(b.max.z, n.max.z)) * 0.5f;
                int cx = int((b.max.x - m_cityOrigin.x) / m_cellSize + 0.5f); // right edge
                int cz = int((midZ - m_cityOrigin.z) / m_cellSize + 0.5f);
                if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                    m_extraSeeds.push_back(glm::ivec2(cx, cz));

                cx = int((b.min.x - m_cityOrigin.x) / m_cellSize + 0.5f); // left edge
                if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                    m_extraSeeds.push_back(glm::ivec2(cx, cz));
            }

            if (neighborZ) {
                // Vertical neighbor: add mid-edge horizontal seeds
                float midX = (std::max(b.min.x, n.min.x) + std::min(b.max.x, n.max.x)) * 0.5f;
                int cz = int((b.max.z - m_cityOrigin.z) / m_cellSize + 0.5f); // bottom edge
                int cx = int((midX - m_cityOrigin.x) / m_cellSize + 0.5f);
                if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                    m_extraSeeds.push_back(glm::ivec2(cx, cz));

                cz = int((b.min.z - m_cityOrigin.z) / m_cellSize + 0.5f); // top edge
                if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                    m_extraSeeds.push_back(glm::ivec2(cx, cz));
            }
        }
    }
}*/
void RoadNetwork::addSeedsFromBlocks(const std::vector<BuildingBounds>& bounds) {
    m_extraSeeds.clear();

    for (auto &b : bounds) {
        // Corners
        glm::vec3 corners[4] = {
            {b.min.x, 0.0f, b.min.z},
            {b.max.x, 0.0f, b.min.z},
            {b.max.x, 0.0f, b.max.z},
            {b.min.x, 0.0f, b.max.z}
        };

        for (auto &c : corners) {
            int cx = int((c.x - m_cityOrigin.x) / m_cellSize + 0.5f);
            int cz = int((c.z - m_cityOrigin.z) / m_cellSize + 0.5f);
            if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                m_extraSeeds.push_back(glm::ivec2(cx, cz));
        }

        // Mid-edge seeds
        glm::vec3 mid_edges[4] = {
            {(b.min.x + b.max.x) * 0.5f, 0.0f, b.min.z}, // top
            {(b.min.x + b.max.x) * 0.5f, 0.0f, b.max.z}, // bottom
            {b.min.x, 0.0f, (b.min.z + b.max.z) * 0.5f}, // left
            {b.max.x, 0.0f, (b.min.z + b.max.z) * 0.5f}  // right
        };

        for (auto &m : mid_edges) {
            int cx = int((m.x - m_cityOrigin.x) / m_cellSize + 0.5f);
            int cz = int((m.z - m_cityOrigin.z) / m_cellSize + 0.5f);
            if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                m_extraSeeds.push_back(glm::ivec2(cx, cz));
        }
    }
}

// Helper function
void RoadNetwork::addSeedToGrid(float worldX, float worldZ) {
    int cx = static_cast<int>(std::round((worldX - m_cityOrigin.x) / m_cellSize));
    int cz = static_cast<int>(std::round((worldZ - m_cityOrigin.z) / m_cellSize));

    if (cx >= 0 && cx < m_gridW && cz >= 0 && cz < m_gridH) {
        if (m_grid[idx(cx, cz)] == CellType::Empty) {
            m_extraSeeds.push_back(glm::ivec2(cx, cz));
        }
    }
}

void RoadNetwork::mark_buildings_as_blocked(const std::vector<BuildingBounds>& bounds) {
    for (auto &b : bounds) {
        //Convert world coords to cell coords (x -> grid X, z -> grid Y)
        int minX = static_cast<int>(std::floor((b.min.x - m_cityOrigin.x) / m_cellSize));
        int maxX = static_cast<int>(std::floor((b.max.x - m_cityOrigin.x) / m_cellSize));
        int minY = static_cast<int>(std::floor((b.min.z - m_cityOrigin.z) / m_cellSize));
        int maxY = static_cast<int>(std::floor((b.max.z - m_cityOrigin.z) / m_cellSize));
        //Clamp
        minX = std::max(0, std::min(minX, m_gridW-1));
        maxX = std::max(0, std::min(maxX, m_gridW-1));
        minY = std::max(0, std::min(minY, m_gridH-1));
        maxY = std::max(0, std::min(maxY, m_gridH-1));
        for (int y = minY; y <= maxY; ++y) {
            for (int x = minX; x <= maxX; ++x) {
                m_grid[idx(x,y)] = CellType::Building;
            }
        }
    }
}

void RoadNetwork::mark_seed_cells(const std::vector<glm::vec3>& seeds, std::vector<glm::ivec2>& outSeedCells) {
    outSeedCells.clear();
    for (auto &s : seeds) {
        int cx = static_cast<int>(std::round((s.x - m_cityOrigin.x) / m_cellSize));
        int cy = static_cast<int>(std::round((s.z - m_cityOrigin.z) / m_cellSize));
        if (!in_bounds(cx, cy)) continue;
        //If the seed cell is building occupied, try to find nearest empty neighbor
        if (m_grid[idx(cx,cy)] == CellType::Building) {
            bool found = false;
            const int R = 3;
            for (int r = 1; r <= R && !found; ++r) {
                for (int oy = -r; oy <= r && !found; ++oy) {
                    for (int ox = -r; ox <= r && !found; ++ox) {
                        int nx = cx + ox;
                        int ny = cy + oy;
                        if (!in_bounds(nx, ny)) continue;
                        if (m_grid[idx(nx,ny)] == CellType::Empty) {
                            outSeedCells.push_back(glm::ivec2(nx, ny));
                            found = true;
                        }
                    }
                }
            }
            if (!found) continue;
        } else {
            outSeedCells.push_back(glm::ivec2(cx, cy));
        }
    }
}

// ---------- A* pathfinding ----------
struct NodeKey {
    int x, y;
    bool operator==(NodeKey const& o) const { return x==o.x && y==o.y; }
};
struct NodeKeyHash {
    std::size_t operator()(NodeKey const& k) const noexcept { return std::hash<int>()((k.x<<16) ^ k.y); }
};

static inline float heuristic(const glm::ivec2& a, const glm::ivec2& b) {
    //Currently Manhatten distance, try Euclidean later and see results
    return std::abs(a.x - b.x) + std::abs(a.y - b.y);
}

std::vector<glm::ivec2> RoadNetwork::find_path_astar(const glm::ivec2& start, const glm::ivec2& goal) {
    std::vector<glm::ivec2> empty;

    if (!in_bounds(start.x, start.y) || !in_bounds(goal.x, goal.y)) return empty;
    if (m_grid[idx(start.x,start.y)] == CellType::Building) return empty;
    if (m_grid[idx(goal.x,goal.y)] == CellType::Building) return empty;

    //Priority queue of PQItem 
    std::priority_queue<PQItem> open;

    std::unordered_map<NodeKey, glm::ivec2, NodeKeyHash> came_from;
    std::unordered_map<NodeKey, float, NodeKeyHash> gscore;

    NodeKey s{start.x, start.y};
    gscore[s] = 0.0f;
    //Push starting node 
    open.push(PQItem{ heuristic(start, goal), start }); 
    const int dirs[4][2] = { { 1, 0 }, { -1, 0 }, { 0, 1 }, { 0, -1 } }; 
    int max_iterations = m_gridW * m_gridH * 4; 
    int iterations = 0; 
    while (!open.empty() && iterations++ < max_iterations) { PQItem current_item = open.top(); 
        open.pop(); glm::ivec2 current = current_item.pos; //Goal reached then reconstruct path 
        if (current == goal) { std::vector<glm::ivec2> path; 
            glm::ivec2 cur = goal; 
            while (!(cur.x == start.x && cur.y == start.y)) { path.push_back(cur); NodeKey k{ cur.x, cur.y }; 
            if (came_from.find(k) == came_from.end()) break; 
            cur = came_from[k]; 
        } 
        path.push_back(start); 
        std::reverse(path.begin(), path.end()); 
        return path; 
    } //Explore neighbors 
    for (auto& d : dirs) { int nx = current.x + d[0]; int ny = current.y + d[1]; 
        if (!in_bounds(nx, ny)) continue; 
        if (m_grid[idx(nx,ny)] == CellType::Building) continue; 
        glm::ivec2 neighbor(nx, ny); NodeKey nk{nx, ny}; 
        float tentative_g = gscore[{current.x,current.y}] + 1.0f; 
        if (gscore.find(nk) == gscore.end() || tentative_g < gscore[nk]) { came_from[nk] = current; gscore[nk] = tentative_g; 
            float f = tentative_g + heuristic(neighbor, goal); open.push(PQItem{ f, neighbor }); 
        } 
    } 
}
    //No path found
    return empty;
}

//Convert discrete path cells into marking grid cells as road
void RoadNetwork::rasterize_paths_to_roads(const std::vector<std::vector<glm::ivec2>>& paths) {
    for (auto &p : paths) {
        for (auto &c : p) {
            if (in_bounds(c.x, c.y) && m_grid[idx(c.x, c.y)] == CellType::Empty) {
                m_grid[idx(c.x, c.y)] = CellType::Road;
            }
            //Also mark neighbors to make roads thicker, 1 cell radius
            const int R = 1;
            for (int oy = -R; oy <= R; ++oy) {
                for (int ox = -R; ox <= R; ++ox) {
                    int nx = c.x + ox;
                    int ny = c.y + oy;
                    if (!in_bounds(nx, ny)) continue;
                    if (m_grid[idx(nx,ny)] == CellType::Empty) m_grid[idx(nx,ny)] = CellType::Road;
                }
            }
        }
    }
}

//Create geometry, convert contiguous horizontal/vertical runs of road cells into quads
void RoadNetwork::create_meshes_from_roads(float road_width) {
    //Collect all quads into a single mesh
    std::vector<glm::vec3> verts;
    std::vector<GLuint> inds;
    verts.reserve(1024);
    inds.reserve(1024);

    //Create runs horizontally and vertically and add segment quads between centers of adjacent road cells
    for (int y = 0; y < m_gridH; ++y) {
        for (int x = 0; x < m_gridW; ++x) {
            if (m_grid[idx(x,y)] != CellType::Road) continue;
            //For each road cell, create quads linking to right and down neighbors, avoid duplicates
            int rx = x;
            int ry = y;
            glm::vec3 centerA = m_cityOrigin + glm::vec3(rx * m_cellSize + m_cellSize*0.5f, 0.0f, ry * m_cellSize + m_cellSize*0.5f);

            //Right neighbor
            int nx = rx + 1;
            int ny = ry;
            if (in_bounds(nx, ny) && m_grid[idx(nx,ny)] == CellType::Road) {
                glm::vec3 centerB = m_cityOrigin + glm::vec3(nx * m_cellSize + m_cellSize*0.5f, 0.0f, ny * m_cellSize + m_cellSize*0.5f);
                push_road_segment_geom(verts, inds, centerA, centerB, road_width);
            }
            //Down neighbor
            nx = rx;
            ny = ry + 1;
            if (in_bounds(nx, ny) && m_grid[idx(nx,ny)] == CellType::Road) {
                glm::vec3 centerB = m_cityOrigin + glm::vec3(nx * m_cellSize + m_cellSize*0.5f, 0.0f, ny * m_cellSize + m_cellSize*0.5f);
                push_road_segment_geom(verts, inds, centerA, centerB, road_width);
            }
        }
    }

    if (verts.empty() || inds.empty()) return;

    //Upload one mesh
    RoadMesh mesh;
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);

    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, verts.size() * sizeof(glm::vec3), verts.data(), GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,sizeof(glm::vec3),(void*)0);

    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, inds.size() * sizeof(GLuint), inds.data(), GL_STATIC_DRAW);

    glBindVertexArray(0);

    mesh.index_count = static_cast<GLsizei>(inds.size());
    m_meshes.push_back(mesh);
}

void RoadNetwork::push_road_segment_geom(std::vector<glm::vec3>& verts, std::vector<GLuint>& inds, const glm::vec3& a, const glm::vec3& b, float width) {
    glm::vec3 dir = b - a;
    dir.y = 0.0f;
    float len = glm::length(dir);
    if (len <= 1e-6f) return;
    dir = dir / len;
    glm::vec3 right = glm::normalize(glm::cross(dir, glm::vec3(0.0f,1.0f,0.0f))) * (width * 0.5f);

    //Create 4 verts, a+right, a-right, b+right, b-right
    GLuint base = static_cast<GLuint>(verts.size());
    verts.push_back(a + right);
    verts.push_back(a - right);
    verts.push_back(b + right);
    verts.push_back(b - right);

    //Indices
    inds.push_back(base + 0);
    inds.push_back(base + 1);
    inds.push_back(base + 2);

    inds.push_back(base + 2);
    inds.push_back(base + 1);
    inds.push_back(base + 3);
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