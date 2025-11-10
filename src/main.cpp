#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "../samplefw/SampleRunner.h"

#include "city_buildings_gui.h"
#include "city_buildings.h"

class Week2 : public wolf::App {
public:
    Week2() : App("City Blocks Demo") {
        m_city = new CityBuildings(this);
        m_city->init();
        m_city->generateRandomCity(10, 20, 2, 5); //initial city

        m_gui = new CityBuildingsGUI(this);
        m_gui->setCubeRenderer(m_city);

        m_rDown = false;
    }

    ~Week2() {
        delete m_gui;
        delete m_city;
    }

    void update(float dt) override {
        //Reset with new city
        if (isKeyDown('R')) {
            if (!m_rDown) {
                m_city->generateRandomCity(10, 20, 2, 5);
                m_rDown = true;
            }
        } else {
            m_rDown = false;
        }

        m_city->update(dt);
        m_gui->update();
    }

    void render() override {
        m_city->render(m_width, m_height);
        m_gui->render();
    }

private:
    CityBuildings* m_city;
    CityBuildingsGUI* m_gui;

    bool m_rDown; //Tracks R key state
};

int main(int, char**) {
    Week2 app;
    app.run();
}