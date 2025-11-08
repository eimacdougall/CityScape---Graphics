#include "cube.h"

CubeGUI::CubeGUI(wolf::App* app)
    : m_app(app), m_selectedCube(0), m_cubeRenderer(nullptr)
{
    //Initialize ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

    ImGui::StyleColorsDark();

    //Get GLFW window from Wolf
    GLFWwindow* window = m_app->getWindow();

    //Initialize ImGui backends using Wolf's window
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

CubeGUI::~CubeGUI()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void CubeGUI::update()
{
}

void CubeGUI::render() {
    //Start new frame
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    //Cube Controls Window
    ImGui::Begin("Cube Controls");
    
    if (m_cubeRenderer) {
        //GLOBAL CONTROLS
        ImGui::SeparatorText("Global Controls");
        
        //Get reference to the selected cube
        CubeInstance& cube = m_cubeRenderer->getCube(m_selectedCube);
        
        //RESET BUTTONS
        if (ImGui::Button("Reset This Cube")) {
            cube.rotation = glm::vec3(0.0f);
            cube.scale = 1.0f;
            cube.position = glm::vec3(0.0f);
            cube.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            cube.rotationSpeed = glm::vec2(0.0f);
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Reset All Cubes")) {
            for (int i = 0; i < m_cubeRenderer->getCubeCount(); i++) {
                CubeInstance& cube = m_cubeRenderer->getCube(i);
                cube.rotation = glm::vec3(0.0f);
                cube.scale = 1.0f;
                cube.position = glm::vec3(0.0f);
                cube.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
                cube.rotationSpeed = glm::vec2(0.0f);
            }
        }
        
        ImGui::Separator();

        ImGui::Text("Apply rotation speed to all cubes:");
        ImGui::SliderFloat("Global Spin X", &m_globalRotationSpeedX, -360.0f, 360.0f, "%.1f deg/s");
        ImGui::SliderFloat("Global Spin Y", &m_globalRotationSpeedY, -360.0f, 360.0f, "%.1f deg/s");
        
        if (ImGui::Button("Apply to All Cubes"))
        {
            for (int i = 0; i < m_cubeRenderer->getCubeCount(); i++)
            {
                CubeInstance& c = m_cubeRenderer->getCube(i);
                c.rotationSpeed.x = m_globalRotationSpeedX;
                c.rotationSpeed.y = m_globalRotationSpeedY;
            }
        }
        
        ImGui::SameLine();
        
        if (ImGui::Button("Stop All Spinning"))
        {
            for (int i = 0; i < m_cubeRenderer->getCubeCount(); i++)
            {
                CubeInstance& c = m_cubeRenderer->getCube(i);
                c.rotationSpeed = glm::vec2(0.0f);
            }
            m_globalRotationSpeedX = 0.0f;
            m_globalRotationSpeedY = 0.0f;
        }
        
        ImGui::Separator();

        //MANUAL CONTROLS
        ImGui::SeparatorText("Manual Controls");

        //CUBE SELECTOR
        ImGui::Text("Select Cube to Edit:");
        
        //Slider for cube selection
        ImGui::SliderInt("Cube Index", &m_selectedCube, 0, m_cubeRenderer->getCubeCount() - 1);
        
        //Show grid position for selected cube
        int row = m_selectedCube / 3 + 1;
        int col = m_selectedCube % 3 + 1;
        ImGui::Text("Grid Position: Row %d, Column %d", row, col);
        
        ImGui::Separator();

        //ROTATION SPEED CONTROLS
        ImGui::Text("Rotation Speed (Automatic)");
        ImGui::SliderFloat("Spin Speed X", &cube.rotationSpeed.x, -360.0f, 360.0f, "%.1f deg/s");
        ImGui::SliderFloat("Spin Speed Y", &cube.rotationSpeed.y, -360.0f, 360.0f, "%.1f deg/s");
        if (ImGui::Button("Stop Spinning on X Axis")) {
            cube.rotationSpeed.x = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Stop Spinning on Y Axis")) {
            cube.rotationSpeed.y = 0.0f;
        }
        if (ImGui::Button("Stop Spinning")) {
            cube.rotationSpeed = glm::vec2(0.0f);
        }
        
        ImGui::Separator();
        
        //ROTATION CONTROLS
        ImGui::Text("Rotation");
        ImGui::SliderFloat("Rotation X", &cube.rotation.x, 0.0f, 360.0f, "%.1f deg");
        ImGui::SliderFloat("Rotation Y", &cube.rotation.y, 0.0f, 360.0f, "%.1f deg");
        ImGui::SliderFloat("Rotation Z", &cube.rotation.z, 0.0f, 360.0f, "%.1f deg");

        if (ImGui::Button("Reset X")) {
            cube.rotation.x = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Y")) {
            cube.rotation.y = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset Z")) {
            cube.rotation.z = 0.0f;
        }
        ImGui::SameLine();
        if (ImGui::Button("Reset All")) {
            cube.rotation = glm::vec3(0.0f);
        }
        
        ImGui::Separator();
        
        //SCALE CONTROL
        ImGui::Text("Scale");
        ImGui::SliderFloat("Scale", &cube.scale, 0.1f, 5.0f, "%.2f");
        if (ImGui::Button("Reset Scale")) {
            cube.scale = 1.0f;
        }
        
        ImGui::Separator();
        
        //POSITION CONTROL
        ImGui::Text("Position Offset");
        ImGui::SliderFloat3("Position", &cube.position.x, -30.0f, 30.0f);
        if (ImGui::Button("Reset Position")) {
            cube.position = glm::vec3(0.0f);
        }

        ImGui::Separator();

        //COLOR CONTROL
        ImGui::Text("Color");
        ImGui::ColorEdit4("Cube Color", &cube.color.x);
        if (ImGui::Button("Reset Color")) {
            cube.color = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        }
    }
    else
    {
        ImGui::Text("No cube renderer connected!");
        ImGui::Text("Call setCubeRenderer() to enable controls.");
    }
    
    ImGui::Separator();

    ImGui::End();

    //Render ImGui
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}