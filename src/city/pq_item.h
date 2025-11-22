#pragma once
#include <glm/glm.hpp>

// Item stored in A* priority queue
// We define operator< to make std::priority_queue a min-heap based on cost.
struct PQItem {
float cost; // f = g + h
glm::ivec2 pos; // grid position

// Reversed comparison so std::priority_queue returns smallest cost first
bool operator<(const PQItem& other) const {
    return cost > other.cost;
}

};