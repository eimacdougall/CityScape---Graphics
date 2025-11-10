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
    if (!m_cubeRenderer)
        return;

    // Push the GUI values to the CityBuildings instance
    m_cubeRenderer->setSpacing(m_spacing);
    m_cubeRenderer->setBuildingHeightRange(m_buildingScaleMin, m_buildingScaleMax);
}

void CityBuildingsGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("City Controls");

    if (m_cubeRenderer) {
        ImGui::Text("Global Controls");

        ImGui::SliderFloat("Spacing", &m_spacing, 2.0f, 15.0f);
        ImGui::SliderFloat("Min Building Height", &m_buildingScaleMin, 1.0f, 10.0f);
        ImGui::SliderFloat("Max Building Height", &m_buildingScaleMax, 5.0f, 20.0f);

        if (ImGui::Button("Reset Global")) {
            m_spacing = 7.0f;
            m_buildingScaleMin = 2.0f;
            m_buildingScaleMax = 12.0f;
        }
    } else {
        ImGui::Text("No cube renderer connected!");
        ImGui::Text("Call setCubeRenderer() to enable controls.");
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}