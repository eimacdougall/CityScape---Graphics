#include "road.h"

void RoadNetwork::mark_buildings_as_blocked(const std::vector<BuildingBounds>& bounds, int buffer) {
    for (auto &b : bounds) {
        int minX = int(std::floor((b.min.x - m_cityOrigin.x) / m_cellSize)) - buffer;
        int maxX = int(std::floor((b.max.x - m_cityOrigin.x) / m_cellSize)) + buffer;
        int minY = int(std::floor((b.min.z - m_cityOrigin.z) / m_cellSize)) - buffer;
        int maxY = int(std::floor((b.max.z - m_cityOrigin.z) / m_cellSize)) + buffer;

        minX = std::max(0, std::min(minX, m_gridW-1));
        maxX = std::max(0, std::min(maxX, m_gridW-1));
        minY = std::max(0, std::min(minY, m_gridH-1));
        maxY = std::max(0, std::min(maxY, m_gridH-1));

        for (int y = minY; y <= maxY; ++y)
            for (int x = minX; x <= maxX; ++x)
                m_grid[idx(x,y)] = CellType::Building;
    }
}

void RoadNetwork::mark_seed_cells(const std::vector<glm::vec3>& seeds, std::vector<glm::ivec2>& outSeedCells) {
    outSeedCells.clear();
    for (auto &s : seeds) {
        int cx = static_cast<int>(std::round((s.x - m_cityOrigin.x) / m_cellSize));
        int cy = static_cast<int>(std::round((s.z - m_cityOrigin.z) / m_cellSize));
        if (!in_bounds(cx, cy)) continue;

        //If the seed is inside a building, try a few times more on nearby cells
        if (m_grid[idx(cx,cy)] == CellType::Building) {
            bool found = false;
            const int R = 3;
            //Expanding square search outwards until an empty cell is found
            for (int r = 1; r <= R && !found; ++r) {
                for (int oy = -r; oy <= r && !found; ++oy) {
                    for (int ox = -r; ox <= r && !found; ++ox) {
                        int nx = cx + ox;
                        int ny = cy + oy;
                        if (!in_bounds(nx, ny)) continue;
                        if (m_grid[idx(nx, ny)] == CellType::Empty) {
                            outSeedCells.push_back(glm::ivec2(nx, ny)); //Becomes the seed
                            found = true;
                        }
                    }
                }
            }
        } else {
            outSeedCells.push_back(glm::ivec2(cx, cy));
        }
    }
}

void RoadNetwork::addSeedsFromBlocks(const std::vector<BuildingBounds>& bounds) {
    m_extraSeeds.clear();

    for (auto &b : bounds) {
        //Corner seeds
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

        //Mid-edge seeds (two per edge, opposite directions)
        glm::vec3 mid_edges[4][2] = {
            //Top edge, left and right offset
            { {(b.min.x + b.max.x) * 0.5f, 0.0f, b.min.z - m_cellSize},
              {(b.min.x + b.max.x) * 0.5f, 0.0f, b.min.z + m_cellSize} },
            //Bottom edge
            { {(b.min.x + b.max.x) * 0.5f, 0.0f, b.max.z - m_cellSize},
              {(b.min.x + b.max.x) * 0.5f, 0.0f, b.max.z + m_cellSize} },
            //Left edge
            { {b.min.x - m_cellSize, 0.0f, (b.min.z + b.max.z) * 0.5f},
              {b.min.x + m_cellSize, 0.0f, (b.min.z + b.max.z) * 0.5f} },
            //Right edge
            { {b.max.x - m_cellSize, 0.0f, (b.min.z + b.max.z) * 0.5f},
              {b.max.x + m_cellSize, 0.0f, (b.min.z + b.max.z) * 0.5f} }
        };

        for (auto &pair : mid_edges) {
            for (auto &m : pair) {
                int cx = int((m.x - m_cityOrigin.x) / m_cellSize + 0.5f);
                int cz = int((m.z - m_cityOrigin.z) / m_cellSize + 0.5f);
                if (in_bounds(cx, cz) && m_grid[idx(cx, cz)] == CellType::Empty)
                    m_extraSeeds.push_back(glm::ivec2(cx, cz));
            }
        }
    }
}


void RoadNetwork::addSeedToGrid(float worldX, float worldZ) {
    int cx = static_cast<int>(std::round((worldX - m_cityOrigin.x) / m_cellSize));
    int cz = static_cast<int>(std::round((worldZ - m_cityOrigin.z) / m_cellSize));
    if (cx >= 0 && cx < m_gridW && cz >= 0 && cz < m_gridH) {
        if (m_grid[idx(cx, cz)] == CellType::Empty)
            m_extraSeeds.push_back(glm::ivec2(cx, cz));
    }
}