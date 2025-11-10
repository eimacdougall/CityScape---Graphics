#pragma once
#include "../wolf/wolf.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "city_buildings.h"

// Forward declaration
class CityBuildings;

class CityBuildingsGUI {
public:
    CityBuildingsGUI(wolf::App* app);
    ~CityBuildingsGUI();

    void setCubeRenderer(CityBuildings* cube) { m_cubeRenderer = cube; }

    void update();
    void render();

private:
    wolf::App* m_app = nullptr;
    CityBuildings* m_cubeRenderer = nullptr;

    // Global uniforms
    float m_spacing = 4.0f;
    float m_buildingScaleMin = 2.0f;
    float m_buildingScaleMax = 12.0f;

    // Block controls
    glm::vec3 m_newBlockOrigin = glm::vec3(0.0f);
    int m_newBlockWidth = 4;
    int m_newBlockDepth = 4;
};