#include "city_buildings_gui.h"

CityBuildingsGUI::CityBuildingsGUI(wolf::App* app)
    : m_app(app)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    GLFWwindow* window = m_app->getWindow();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");
}

CityBuildingsGUI::~CityBuildingsGUI() {
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}

void CityBuildingsGUI::update() {
    if (!m_cubeRenderer || !m_cubeRenderer->getProgram())
        return;

    m_cubeRenderer->setMinBuildingGap(m_minBuildingGap);
    m_cubeRenderer->setBuildingHeightRange(m_buildingScaleMin, m_buildingScaleMax);
    m_cubeRenderer->setBuildingSizeRange(m_buildingWidthMin, m_buildingWidthMax,
                                         m_buildingDepthMin, m_buildingDepthMax);
}

void CityBuildingsGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("City Controls");

    if (m_cubeRenderer) {
        ImGui::SliderFloat("Min Building Gap", &m_minBuildingGap, 0.5f, 10.0f);

        ImGui::SliderFloat("Min Height", &m_buildingScaleMin, 1.0f, 10.0f);
        ImGui::SliderFloat("Max Height", &m_buildingScaleMax, 5.0f, 20.0f);

        ImGui::SliderFloat("Width Min", &m_buildingWidthMin, 1.0f, 5.0f);
        ImGui::SliderFloat("Width Max", &m_buildingWidthMax, 2.0f, 10.0f);
        ImGui::SliderFloat("Depth Min", &m_buildingDepthMin, 1.0f, 5.0f);
        ImGui::SliderFloat("Depth Max", &m_buildingDepthMax, 2.0f, 10.0f);

        if (ImGui::Button("Reset Global")) {
            m_blockSpacing = 10.0f;
            m_minBuildingGap = 2.5f;
            m_buildingScaleMin = 1.0f;
            m_buildingScaleMax = 5.0f;
            m_buildingWidthMin = 1.0f;
            m_buildingWidthMax = 3.0f;
            m_buildingDepthMin = 1.0f;
            m_buildingDepthMax = 3.0f;
        }

        ImGui::SeparatorText("Camera Options");
        if (m_camera) {
            ImGui::SliderFloat("Camera Speed", &m_cameraSpeed, 1.0f, 200.0f);
            m_camera->setSpeed(m_cameraSpeed);
        }

    } else {
        ImGui::Text("No cube renderer connected!");
    }

    ImGui::End();
    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}