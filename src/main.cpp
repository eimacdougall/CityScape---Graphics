#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "../samplefw/SampleRunner.h"

#include "gui/city_buildings_gui.h"
#include "city/city_buildings.h"
#include "camera/fly_camera.h"
#include "city/road/road.h"

class City : public wolf::App {
public:
    City() : App("City Blocks Demo") {
        //Load shaders
        m_cityShader = wolf::LoadShaders("data/city_buildings.vsh", "data/city_buildings.fsh");
        m_sidewalkShader = wolf::LoadShaders("data/sidewalk.vsh", "data/sidewalk.fsh");
        m_roadShader = wolf::LoadShaders("data/road.vsh", "data/road.fsh");

        //Create multiple city instances
        m_city1 = new CityBuildings(this, m_cityShader, m_sidewalkShader, m_roadShader);
        m_city2 = new CityBuildings(this, m_cityShader, m_sidewalkShader, m_roadShader);

        m_city1->init();
        m_city2->init();

        //Set city origins
        m_city1->setCityOrigin(glm::vec3(0.0f, 0.0f, 0.0f));
        m_city2->setCityOrigin(glm::vec3(800.0f, 0.0f, 0.0f));

        //Generate random cities
        m_city1->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
        m_city2->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);

        //Camera & GUI
        m_pFlyCam = new FlyCamera(this);
        m_gui = new CityBuildingsGUI(this);
        m_gui->setCubeRenderer(m_city1);
        m_gui->setCamera(m_pFlyCam);

        m_rDown = false;
    }

    ~City() {
        delete m_gui;
        delete m_city1;
        delete m_city2;
        delete m_pFlyCam;

        glDeleteProgram(m_cityShader);
        glDeleteProgram(m_sidewalkShader);
        glDeleteProgram(m_roadShader);
    }

    void update(float dt) override {
        if (isKeyDown('R') && !m_rDown) {
            m_city1->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
            m_city2->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
            m_rDown = true;
        } else if (!isKeyDown('R')) {
            m_rDown = false;
        }

        if (m_pFlyCam) m_pFlyCam->update(dt);

        m_city1->update(dt);
        m_city2->update(dt);
        m_gui->update();
    }

    void render() override {
        glViewport(0, 0, m_width, m_height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 viewProj = m_pFlyCam->getProjMatrix(m_width, m_height) * m_pFlyCam->getViewMatrix();

        //Render cities
        float maxDistance = 500.0f;
        m_city1->render(viewProj, m_pFlyCam->getPosition(), maxDistance);
        m_city2->render(viewProj, m_pFlyCam->getPosition(), maxDistance);

        m_gui->render();
    }

private:
    CityBuildings* m_city1;
    CityBuildings* m_city2;
    CityBuildingsGUI* m_gui;
    FlyCamera* m_pFlyCam = nullptr;

    GLuint m_cityShader;
    GLuint m_sidewalkShader;
    GLuint m_roadShader;

    bool m_rDown;

    static constexpr int minBlocks = 10;
    static constexpr int maxBlocks = 20;
    static constexpr int minBlockSize = 2;
    static constexpr int maxBlockSize = 5;
};

int main(int, char**) {
    City app;
    app.run();
}