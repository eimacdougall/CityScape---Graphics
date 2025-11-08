#pragma once
#include "../wolf/wolf.h"
#include "imgui.h"
#include "backends/imgui_impl_glfw.h"
#include "backends/imgui_impl_opengl3.h"

#include "thirtyCubes.h"

//Forward declaration
class ThirtyCubesInstanced;
struct CubeInstance;

class CubeGUI {
public:
    CubeGUI(wolf::App* app);
    ~CubeGUI();

    void update();
    void render();
    
    //Connect the cube renderer so GUI can access cube data
    void setCubeRenderer(ThirtyCubesInstanced* renderer) { 
        m_cubeRenderer = renderer; 
    }

private:
    wolf::App* m_app;
    ThirtyCubesInstanced* m_cubeRenderer;
    
    int m_selectedCube;
    float m_globalRotationSpeedX = 0.0f;
    float m_globalRotationSpeedY = 0.0f;
};