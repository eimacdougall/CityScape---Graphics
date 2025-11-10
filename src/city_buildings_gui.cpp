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

    glUseProgram(m_cubeRenderer->getProgram());
    glUniform1f(glGetUniformLocation(m_cubeRenderer->getProgram(), "u_spacing"), m_spacing);
    glUniform1f(glGetUniformLocation(m_cubeRenderer->getProgram(), "u_buildingScaleMin"), m_buildingScaleMin);
    glUniform1f(glGetUniformLocation(m_cubeRenderer->getProgram(), "u_buildingScaleMax"), m_buildingScaleMax);
}

void CityBuildingsGUI::render() {
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();
    ImGui::NewFrame();

    ImGui::Begin("City Controls");

    if (m_cubeRenderer) {
        ImGui::Text("Global Controls");
        ImGui::SliderFloat("Spacing", &m_spacing, 2.0f, 15.0f);
        ImGui::SliderFloat("Min Height", &m_buildingScaleMin, 1.0f, 10.0f);
        ImGui::SliderFloat("Max Height", &m_buildingScaleMax, 5.0f, 20.0f);

        ImGui::Separator();
        ImGui::Text("Add / Remove Blocks");

        ImGui::InputFloat3("New Block Origin", &m_newBlockOrigin.x);
        ImGui::SliderInt("Block Width", &m_newBlockWidth, 3, 5);
        ImGui::SliderInt("Block Depth", &m_newBlockDepth, 3, 5);

        if (ImGui::Button("Add Block")) {
            m_cubeRenderer->addBlock(m_newBlockOrigin, m_newBlockWidth, m_newBlockDepth);
        }

        if (ImGui::Button("Remove Last Block")) {
            m_cubeRenderer->removeLastBlock();
        }

        if (ImGui::Button("Clear All Blocks")) {
            m_cubeRenderer->clearBlocks();
        }

    } else {
        ImGui::Text("No cube renderer connected!");
        ImGui::Text("Call setCubeRenderer() to enable controls.");
    }

    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}