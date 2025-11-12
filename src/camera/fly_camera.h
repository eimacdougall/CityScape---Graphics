#pragma once
#include "../wolf/wolf.h"
#include <GLFW/glfw3.h>

class FlyCamera
{
public:
    FlyCamera(wolf::App* pApp);
    virtual ~FlyCamera();

    void update(float dt);
    glm::mat4 getViewMatrix();
    glm::mat4 getProjMatrix(int width, int height);

    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getForward() const;
    glm::vec3 getRight() const;

    void setSpeed(float speed) { m_speed = speed; }
    float getSpeed() const { return m_speed; }

    void enableMouseLock(bool enable); //This may be used later for accessing gui controls

private:
    void _processMouse();
    void _processKeyboard(float dt);
    void _handleMouseToggle();

    wolf::App* m_pApp = nullptr;

    glm::vec3 m_position = glm::vec3(0.0f, 2.0f, 5.0f);
    float m_yaw = -90.0f;     
    float m_pitch = 0.0f;
    float m_speed = 50.0f;
    float m_sensitivity = 0.1f;

    glm::vec2 m_lastMousePos = glm::vec2(0.0f);
    bool m_firstMouse = true;
    bool m_mouseLocked = false;
    bool m_prevRMBState = false;

    float m_fov = glm::radians(60.0f);
    float m_near = 0.1f;
    float m_far = 2000.0f;
};