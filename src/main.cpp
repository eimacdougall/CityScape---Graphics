#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "../samplefw/SampleRunner.h"

#include "city_buildings_gui.h"
#include "city_buildings.h"

class Week2 : public wolf::App
{
public:
Week2() : App("Week 4")
{
    m_cube = new CityBuildings(this);
    m_sampleRunner.addSample(m_cube);

    m_gui = new CityBuildingsGUI(this);
    m_gui->setCubeRenderer(m_cube);

    // Add initial blocks
    m_cube->addBlock(glm::vec3(0, 0, 0), 4, 4);     // 4x4 block at origin
    m_cube->addBlock(glm::vec3(30, 0, 0), 5, 3);    // 5x3 block offset on X
}

    ~Week2() {
        delete m_gui;
        delete m_cube;
    }

    void update(float dt) override
    {
        if (isKeyDown(' ')) {
            m_lastDown = true;
        }
        else if (m_lastDown) {
            m_sampleRunner.nextSample();
            m_lastDown = false;
        }

        m_sampleRunner.update(dt);
        m_gui->update();
    }

    void render() override
    {
        m_sampleRunner.render(m_width, m_height);
        m_gui->render();
    }

private:
    SampleRunner m_sampleRunner;
    bool m_lastDown = false;

    CityBuildingsGUI* m_gui;
    CityBuildings* m_cube;
};

int main(int, char**) {
    Week2 week2;
    week2.run();
}