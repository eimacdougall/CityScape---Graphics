#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "../samplefw/SampleRunner.h"

#include "city_buildings_gui.h"
#include "city_buildings.h"

class City : public wolf::App {
public:
    City() : App("City Blocks Demo") {
        //Load shared shader
        m_cityShader = wolf::LoadShaders("data/city_buildings.vsh", "data/city_buildings.fsh");

        //Create multiple city instances
        m_city1 = new CityBuildings(this, m_cityShader);
        m_city2 = new CityBuildings(this, m_cityShader);

        m_city1->init();
        m_city2->init();

        m_city1->setCityOrigin(glm::vec3(0.0f, 0.0f, 0.0f));
        m_city2->setCityOrigin(glm::vec3(300.0f, 0.0f, 0.0f));

        m_city1->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
        m_city2->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);

        m_gui = new CityBuildingsGUI(this);
        m_gui->setCubeRenderer(m_city1);

        m_rDown = false;
    }

    ~City() {
        delete m_gui;
        delete m_city1;
        delete m_city2;
        glDeleteProgram(m_cityShader);
    }

    void update(float dt) override {
        if (isKeyDown('R')) {
            if (!m_rDown) {
                m_city1->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
                m_city2->generateRandomCity(minBlocks, maxBlocks, minBlockSize, maxBlockSize);
                m_rDown = true;
            }
        } else {
            m_rDown = false;
        }

        m_city1->update(dt);
        m_city2->update(dt);
        m_gui->update();
    }

    void render() override {
        glViewport(0, 0, m_width, m_height);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LEQUAL);
        glClearColor(0.15f,0.15f,0.15f,1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        m_city1->render(m_width, m_height);
        m_city2->render(m_width, m_height);
        m_gui->render();
    }

private:
    CityBuildings* m_city1;
    CityBuildings* m_city2;
    CityBuildingsGUI* m_gui;
    GLuint m_cityShader;

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