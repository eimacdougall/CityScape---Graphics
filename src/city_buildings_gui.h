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

    void update();
    void render();

    void setCubeRenderer(CityBuildings* renderer) { 
        m_cubeRenderer = renderer; 
    }

private:
    wolf::App* m_app = nullptr;
    CityBuildings* m_cubeRenderer = nullptr;

    // GUI-controlled parameters
    float m_spacing = 7.0f;
    float m_buildingScaleMin = 2.0f;
    float m_buildingScaleMax = 14.0f;
};