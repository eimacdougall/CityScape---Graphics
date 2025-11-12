#pragma once
#include "../wolf/wolf.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "../city/city_buildings.h"
#include "../camera/fly_camera.h"

//Forward declaration
class CityBuildings;
class FlyCamera;

class CityBuildingsGUI {
public:
    CityBuildingsGUI(wolf::App* app);
    ~CityBuildingsGUI();

    void setCubeRenderer(CityBuildings* renderer) { m_cubeRenderer = renderer; }
    void setCamera(FlyCamera* camera) { m_camera = camera; }

    void update();
    void render();

private:
    wolf::App* m_app;
    CityBuildings* m_cubeRenderer = nullptr;
    FlyCamera* m_camera = nullptr;

    float m_blockSpacing = 1.0f; //Distance between blocks
    float m_minBuildingGap = 2.5f; //Minimum gap between buildings in a block
    float m_buildingScaleMin = 1.0f;
    float m_buildingScaleMax = 5.0f;

    float m_buildingWidthMin = 1.0f;
    float m_buildingWidthMax = 3.0f;
    float m_buildingDepthMin = 1.0f;
    float m_buildingDepthMax = 3.0f;

    float m_cameraSpeed = 50.0f;
};