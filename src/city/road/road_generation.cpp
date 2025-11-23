#include "road.h"

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

    //Mark buildings + 1 cell buffer
    mark_buildings_as_blocked(bounds, /*buffer=*/1);

    //Generate initial seeds
    std::vector<glm::ivec2> seed_cells;
    mark_seed_cells(seeds, seed_cells);

    //Add multiple seeds along block edges
    addSeedsFromBlocks(bounds);
    seed_cells.insert(seed_cells.end(), m_extraSeeds.begin(), m_extraSeeds.end());

    if (seed_cells.size() < 2) return;

    //MST connection
    std::vector<std::vector<glm::ivec2>> all_paths;
    std::vector<bool> connected(seed_cells.size(), false);
    connected[0] = true; //Use first seed as the start of the MST
    std::vector<int> connected_ids = {0};

    while (connected_ids.size() < seed_cells.size()) {
        float best_dist = std::numeric_limits<float>::infinity();
        int best_from = -1, best_to = -1;

        //Find closest unconnected seed to any connected seed
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

        //Connect nearest unconnected pair
        auto path = find_path_astar(seed_cells[best_from], seed_cells[best_to]);
        if (!path.empty()) all_paths.push_back(path);

        connected[best_to] = true;
        connected_ids.push_back(best_to);
    }

    //Give each seed a second nearest link to reduce dead ends
    for (size_t i = 0; i < seed_cells.size(); ++i) {
        glm::ivec2 a = seed_cells[i];
        float second_dist = std::numeric_limits<float>::infinity();
        int second_idx = -1;
        //Nearest other seed
        for (size_t j = 0; j < seed_cells.size(); ++j) {
            if (i == j) continue;
            glm::ivec2 b = seed_cells[j];
            float d = glm::distance(glm::vec2(a.x,a.y), glm::vec2(b.x,b.y));
            if (d < second_dist) {
                second_dist = d;
                second_idx = (int)j;
            }
        }
        if (second_idx >= 0) {
            auto path = find_path_astar(seed_cells[i], seed_cells[second_idx]);
            if (!path.empty()) all_paths.push_back(path);
        }
    }

    //Rasterize paths
    rasterize_paths_to_roads(all_paths);

    //Connect adjacent path cells if they have small gaps
    for (auto &path : all_paths) {
        for (size_t i = 1; i < path.size(); ++i) {
            glm::ivec2 a = path[i-1];
            glm::ivec2 b = path[i];
            int dx = b.x - a.x;
            int dy = b.y - a.y;
            int steps = std::max(std::abs(dx), std::abs(dy));
            for (int s = 0; s <= steps; ++s) {
                int x = a.x + (dx * s) / steps;
                int y = a.y + (dy * s) / steps;
                if (in_bounds(x, y) && m_grid[idx(x, y)] == CellType::Empty)
                    m_grid[idx(x, y)] = CellType::Road;
            }
        }
    }

    //Create meshes
    create_meshes_from_roads(3.5f);
}

std::vector<glm::ivec2> RoadNetwork::generate_backbone_seeds(const std::vector<glm::ivec2>& building_seeds, int edge_count, int farthest_count) {
    std::vector<glm::ivec2> backbone;

    float dx = m_gridW / float(edge_count + 1);
    float dz = m_gridH / float(edge_count + 1);

    for (int i = 1; i <= edge_count; ++i) {
        backbone.push_back(glm::ivec2(int(i*dx), 0));
        backbone.push_back(glm::ivec2(int(i*dx), m_gridH-1));
        backbone.push_back(glm::ivec2(0, int(i*dz)));
        backbone.push_back(glm::ivec2(m_gridW-1, int(i*dz)));
    }

    if (!building_seeds.empty()) {
        std::vector<glm::ivec2> selected;
        selected.push_back(building_seeds[0]);

        while ((int)selected.size() < farthest_count && selected.size() < building_seeds.size()) {
            float best_dist = -1.0f;
            int best_idx = -1;
            for (size_t i = 0; i < building_seeds.size(); ++i) {
                bool used = false;
                for (auto &b : selected) if (b == building_seeds[i]) used = true;
                if (used) continue;

                float min_d = std::numeric_limits<float>::infinity();
                for (auto &b : selected)
                    min_d = std::min(min_d, glm::distance(glm::vec2(b.x,b.y), glm::vec2(building_seeds[i].x, building_seeds[i].y)));

                if (min_d > best_dist) { best_dist = min_d; best_idx = (int)i; }
            }
            if (best_idx == -1) break;
            selected.push_back(building_seeds[best_idx]);
        }

        backbone.insert(backbone.end(), selected.begin(), selected.end());
    }

    return backbone;
}

std::vector<RoadNetwork::MSTEdge> RoadNetwork::build_backbone_mst(const std::vector<glm::ivec2>& backbone) {
    std::vector<MSTEdge> mst_edges;
    int n = (int)backbone.size();
    if (n < 2) return mst_edges;

    std::vector<int> parent(n); //Disjoint set for kruskal
    for (int i = 0; i < n; ++i) parent[i] = i;
    auto find = [&](int i){ while (parent[i] != i) i = parent[i]; return i; };
    auto unite = [&](int a, int b){ parent[find(a)] = find(b); };

    struct EdgeTuple { float w; int i,j; std::vector<glm::ivec2> path; };
    std::vector<EdgeTuple> edge_list;

    //Try connecting every seed pair
    for (int i = 0; i < n; ++i) {
        for (int j = i+1; j < n; ++j) {
            std::vector<glm::ivec2> path;
            if (!edge_intersects_building(backbone[i], backbone[j]))
                path = {backbone[i], backbone[j]};
            else
                path = find_path_astar(backbone[i], backbone[j]);
            if (path.empty()) continue;

            //Should help with smaller scale pathing with longer stretches so that it doens't run like shit
            float w = static_cast<float>(path.size());
            w -= glm::distance(glm::vec2(backbone[i].x,backbone[i].y), glm::vec2(backbone[j].x,backbone[j].y)) * 0.5f; //favor long edges
            edge_list.push_back({w, i, j, path});
        }
    }

    // Sort lowest weight to first
    std::sort(edge_list.begin(), edge_list.end(), [](const EdgeTuple &a, const EdgeTuple &b){ return a.w < b.w; });

    //Kruskal MST
    for (auto &e : edge_list) {
        if (find(e.i) != find(e.j)) {
            unite(e.i, e.j);
            mst_edges.push_back({e.i, e.j, e.path});
        }
    }

    return mst_edges;
}

void RoadNetwork::connect_remaining_seeds(const std::vector<glm::ivec2>& all_seeds, const std::vector<glm::ivec2>& backbone) {
    for (auto &seed : all_seeds) {
        bool is_backbone = false;
        for (auto &b : backbone) if (b == seed) { is_backbone = true; break; }
        if (is_backbone) continue;
        //Find nearest existing road cell in the grid
        glm::ivec2 nearest_road = seed;
        float min_dist = std::numeric_limits<float>::infinity();
        for (int y = 0; y < m_gridH; ++y) {
            for (int x = 0; x < m_gridW; ++x) {
                if (m_grid[idx(x,y)] == CellType::Road) {
                    float d = glm::distance(glm::vec2(x,y), glm::vec2(seed.x,seed.y));
                    if (d < min_dist) { min_dist = d; nearest_road = glm::ivec2(x,y); }
                }
            }
        }

        //Connect seed to closest road cell
        auto path = find_path_astar(seed, nearest_road);
        if (!path.empty()) rasterize_paths_to_roads({path});
    }
}

void RoadNetwork::build_connected_roads(const std::vector<glm::ivec2>& all_seeds) {
    if (all_seeds.empty()) return;

    auto backbone = generate_backbone_seeds(all_seeds, 4, 8);
    auto backbone_mst = build_backbone_mst(backbone);

    std::vector<std::vector<glm::ivec2>> paths;
    for (auto &e : backbone_mst) paths.push_back(e.path);
    rasterize_paths_to_roads(paths);

    connect_remaining_seeds(all_seeds, backbone);
    create_meshes_from_roads(3.5f);
}
