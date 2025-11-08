#include <stdio.h>
#include <iostream>
#include <glm/glm.hpp>
#include "../wolf/wolf.h"
#include "../samplefw/SampleRunner.h"

#include "cube.h"
#include "thirtyCubes.h"

class Week2: public wolf::App
{
public:
    Week2() : App("Week 4")
    {
        m_cube = new ThirtyCubesInstanced(this);
        m_sampleRunner.addSample(m_cube);
    
        m_gui = new CubeGUI(this);
        m_gui->setCubeRenderer(m_cube);
    }

    ~Week2() {
        delete m_gui;
    }

    void update(float dt) override
    {
        if(isKeyDown(' '))
        {
            m_lastDown = true;
        }
        else if(m_lastDown)
        {
            m_sampleRunner.nextSample();
            m_lastDown = false;
        }

        m_sampleRunner.update(dt);
        m_gui->update();
    }

    void render() override {
    m_sampleRunner.render(m_width, m_height);
    m_gui->render();
    }
   
private:
    SampleRunner m_sampleRunner;
    bool m_lastDown = false;
    CubeGUI* m_gui;
    ThirtyCubesInstanced* m_cube;
};

int main(int, char**) {
    Week2 week2;
    week2.run();
}
