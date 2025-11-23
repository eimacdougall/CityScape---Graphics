#pragma once
#include "../wolf/wolf.h"
#include "../city_block.h"
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

    void build_from_buildings(
        const std::vector<BuildingBounds>& bounds,
        const glm::vec3& city_origin,
        float cell_size,
        int grid_w,
        int grid_h,
        const std::vector<glm::vec3>& seeds = {}
    );

private:
    struct RoadMesh {
        GLuint vao = 0;
        GLuint vbo = 0;
        GLuint ebo = 0;
        GLsizei index_count = 0;
    };

    struct MSTEdge {
        int from_idx;
        int to_idx;
        std::vector<glm::ivec2> path;
    };

    GLuint m_program = 0;
    GLint m_uViewProjLoc = -1;
    GLuint m_uColorLoc = -1;

    //Grid
    enum class CellType : uint8_t { Empty = 0, Building = 1, Road = 2 };
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

    //Helpers
    void RoadNetwork::mark_buildings_as_blocked(const std::vector<BuildingBounds>& bounds, int buffer);
    void mark_seed_cells(const std::vector<glm::vec3>& seeds, std::vector<glm::ivec2>& outSeedCells);
    void addSeedsFromBlocks(const std::vector<BuildingBounds>& bounds);
    void addSeedToGrid(float worldX, float worldZ);

    std::vector<glm::ivec2> find_path_astar(const glm::ivec2& start, const glm::ivec2& goal);
    void rasterize_paths_to_roads(const std::vector<std::vector<glm::ivec2>>& paths);
    void create_meshes_from_roads(float road_width = 3.0f);
    void push_road_segment_geom(std::vector<glm::vec3>& verts, std::vector<GLuint>& inds, const glm::vec3& a, const glm::vec3& b, float width);

    //New high-level road generation
    void build_connected_roads(const std::vector<glm::ivec2>& all_seeds);
    std::vector<glm::ivec2> generate_backbone_seeds(const std::vector<glm::ivec2>& building_seeds, int edge_count = 4, int farthest_count = 8);
    std::vector<MSTEdge> build_backbone_mst(const std::vector<glm::ivec2>& backbone);
    void connect_remaining_seeds(const std::vector<glm::ivec2>& all_seeds, const std::vector<glm::ivec2>& backbone);
    bool RoadNetwork::edge_intersects_building(const glm::ivec2& a, const glm::ivec2& b);
};