#pragma once
#include "../wolf/wolf.h"
#include "city_block.h"
#include "pq_item.h"
#include <queue>
#include <unordered_map>

class RoadNetwork {
public:
    RoadNetwork();
    ~RoadNetwork();

    void cleanup();
    void set_program(GLuint program);
    void render(const glm::mat4& view_proj);

    //Build roads using A* pathfinding on a grid.
    //city_origin: same origin used to place blocks/buildings so coordinates align.
    //cell_size: size of each grid cell in world units.
    //grid_w, grid_h: grid dimensions (number of cells in X and Z)
    //Seeds are an optional list of world-space points to connect (e.g., block centers). If empty, will auto-generate connection seeds.
    void build_from_buildings(
        const std::vector<BuildingBounds>& bounds,
        const glm::vec3& city_origin,
        float cell_size,
        int grid_w,
        int grid_h,
        const std::vector<glm::vec3>& seeds = {}
    );
        
    void RoadNetwork::addSeedsFromBlocks(const std::vector<BuildingBounds>& bounds);
    void RoadNetwork::addSeedToGrid(float worldX, float worldZ);
private:
    struct RoadMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei index_count = 0;
    };

    GLuint m_program = 0;
    GLint m_uViewProjLoc = -1;
    GLuint m_uColorLoc = -1;

    //Grid
    enum class CellType : uint8_t { Empty=0, Building=1, Road=2 };
    int m_gridW = 0;
    int m_gridH = 0;
    float m_cellSize = 1.0f;
    glm::vec3 m_cityOrigin{0.0f,0.0f,0.0f};
    std::vector<CellType> m_grid;
    std::vector<glm::ivec2> m_extraSeeds;

    //Road meshes
    std::vector<RoadMesh> m_meshes;

private:
    inline bool in_bounds(int x, int y) const { return x >= 0 && y >= 0 && x < m_gridW && y < m_gridH; }
    inline int idx(int x, int y) const { return y * m_gridW + x; }

    void mark_buildings_as_blocked(const std::vector<BuildingBounds>& bounds);
    void mark_seed_cells(const std::vector<glm::vec3>& seeds, std::vector<glm::ivec2>& outSeedCells);
    std::vector<glm::ivec2> find_path_astar(const glm::ivec2& start, const glm::ivec2& goal);
    void rasterize_paths_to_roads(const std::vector<std::vector<glm::ivec2>>& paths);
    void create_meshes_from_roads(float road_width = 3.0f);

    //Helper push quad vertices for a single segment (world-space)
    void push_road_segment_geom(std::vector<glm::vec3>& verts, std::vector<GLuint>& inds, const glm::vec3& a, const glm::vec3& b, float width);
};