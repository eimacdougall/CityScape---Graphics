#include "road.h"

//Represents a grid coordinate used as a key in hash maps
struct NodeKey { int x, y; bool operator==(NodeKey const& o) const { return x==o.x && y==o.y; } };
//Hash function for NodeKey so it can be used in unordered map
struct NodeKeyHash { std::size_t operator()(NodeKey const& k) const noexcept { return std::hash<int>()((k.x<<16)^k.y); } }; //Pack x and y into a single int and hash it

static inline float heuristic(const glm::ivec2& a, const glm::ivec2& b) {
    return std::abs(a.x - b.x) + std::abs(a.y - b.y); //Manhattan
}

std::vector<glm::ivec2> RoadNetwork::find_path_astar(const glm::ivec2& start, const glm::ivec2& goal) {
    std::vector<glm::ivec2> empty;
    if (!in_bounds(start.x,start.y) || !in_bounds(goal.x,goal.y)) return empty;
    if (m_grid[idx(start.x,start.y)] == CellType::Building) return empty;
    if (m_grid[idx(goal.x,goal.y)] == CellType::Building) return empty;

    std::priority_queue<PQItem> open; //Priority queue sorted by f score below (g + h)

    //For each visited node:
    //Came_from stores the previous node (for reconstructing the path)
    //G score stores the cost from start to that node
    std::unordered_map<NodeKey, glm::ivec2, NodeKeyHash> came_from;
    std::unordered_map<NodeKey, float, NodeKeyHash> gscore;

    NodeKey s{start.x, start.y};
    gscore[s] = 0.0f;
    open.push(PQItem{heuristic(start,goal), start});

    // 4-connected grid movement directions right, left, down, and up
    const int dirs[4][2] = { {1,0}, {-1,0}, {0,1}, {0,-1} };
    int max_iterations = m_gridW*m_gridH*4; //Safety bound to prevent infinite loops
    int iterations = 0;

    while (!open.empty() && iterations++ < max_iterations) {
        PQItem current_item = open.top(); open.pop(); //Node with lowest f score
        glm::ivec2 current = current_item.pos;
        if (current == goal) { //Goal reached, reconstruct the path by walking backwards
            std::vector<glm::ivec2> path;
            glm::ivec2 cur = goal;
            while (!(cur.x==start.x && cur.y==start.y)) {
                path.push_back(cur);
                NodeKey k{cur.x,cur.y};
                if (came_from.find(k)==came_from.end()) break;
                cur = came_from[k];
            }
            path.push_back(start);
            std::reverse(path.begin(), path.end());
            return path;
        }

        for (auto& d : dirs) { //Explore neighbors in 4 directions
            int nx = current.x + d[0];
            int ny = current.y + d[1];
            if (!in_bounds(nx, ny)) continue;
            if (m_grid[idx(nx,ny)] == CellType::Building) continue;

            glm::ivec2 neighbor(nx,ny);
            NodeKey nk{nx,ny};
            float tentative_g = gscore[{current.x,current.y}] + 1.0f; //Cost from start to neighbor
            if (gscore.find(nk)==gscore.end() || tentative_g < gscore[nk]) { //If new path is better or node has not been visited
                came_from[nk] = current;
                gscore[nk] = tentative_g;
                float f = tentative_g + heuristic(neighbor, goal); //f = g + h
                open.push(PQItem{f, neighbor});
            }
        }
    }

    return empty;
}