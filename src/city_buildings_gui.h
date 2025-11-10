#pragma once
#include "../wolf/wolf.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "city_buildings.h"

//Forward declaration
class CityBuildings;

class CityBuildingsGUI {
public:
    CityBuildingsGUI(wolf::App* app);
    ~CityBuildingsGUI();

    void setCubeRenderer(CityBuildings* renderer) { m_cubeRenderer = renderer; }

    void update();
    void render();

private:
    wolf::App* m_app;
    CityBuildings* m_cubeRenderer = nullptr;

    float m_spacing = 3.0f;
    float m_buildingScaleMin = 2.0f;
    float m_buildingScaleMax = 12.0f;
};