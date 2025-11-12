#include "fly_camera.h"

FlyCamera::FlyCamera(wolf::App* pApp)
    : m_pApp(pApp)
{
    m_lastMousePos = m_pApp->getMousePos();
    enableMouseLock(true); //Lock on startup
}

FlyCamera::~FlyCamera()
{
    enableMouseLock(false);
}

void FlyCamera::update(float dt)
{
    _handleMouseToggle();
    _processMouse();
    _processKeyboard(dt);
}

void FlyCamera::_handleMouseToggle()
{
    bool rmbDown = m_pApp->isRMBDown();
    if (rmbDown && !m_prevRMBState) {
        enableMouseLock(!m_mouseLocked); //Flip lock state
    }

    m_prevRMBState = rmbDown;
}
void FlyCamera::_processMouse()
{
    if (!m_mouseLocked) return;

    glm::vec2 mousePos = m_pApp->getMousePos();

    if (m_firstMouse)
    {
        m_lastMousePos = mousePos;
        m_firstMouse = false;
    }

    glm::vec2 offset = mousePos - m_lastMousePos;
    m_lastMousePos = mousePos;

    offset *= m_sensitivity;

    m_yaw   += offset.x;
    m_pitch -= offset.y;

    //Clamp pitch
    if (m_pitch > 89.0f)  m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    //Recenter cursor every frame if locked
    if (m_mouseLocked)
    {
        GLFWwindow* window = static_cast<GLFWwindow*>(m_pApp->getWindow());
        int width, height;
        glfwGetWindowSize(window, &width, &height);
        glfwSetCursorPos(window, width / 2.0, height / 2.0);
        m_lastMousePos = glm::vec2(width / 2.0f, height / 2.0f);
    }
}

void FlyCamera::_processKeyboard(float dt)
{
    glm::vec3 forward = getForward();
    glm::vec3 right = getRight();

    float velocity = m_speed * dt;

    if (m_pApp->isKeyDown('W')) m_position += forward * velocity;
    if (m_pApp->isKeyDown('S')) m_position -= forward * velocity;
    if (m_pApp->isKeyDown('A')) m_position -= right * velocity;
    if (m_pApp->isKeyDown('D')) m_position += right * velocity;
    if (m_pApp->isKeyDown(' '))  m_position.y += velocity;
    if (m_pApp->isKeyDown('C')) m_position.y -= velocity;
}

glm::mat4 FlyCamera::getViewMatrix()
{
    glm::vec3 forward = getForward();
    return glm::lookAt(m_position, m_position + forward, glm::vec3(0.0f, 1.0f, 0.0f));
}

glm::mat4 FlyCamera::getProjMatrix(int width, int height)
{
    return glm::perspective(m_fov, (float)width / (float)height, m_near, m_far);
}

glm::vec3 FlyCamera::getForward() const
{
    glm::vec3 dir;
    dir.x = cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    dir.y = sin(glm::radians(m_pitch));
    dir.z = sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch));
    return glm::normalize(dir);
}

glm::vec3 FlyCamera::getRight() const
{
    return glm::normalize(glm::cross(getForward(), glm::vec3(0.0f, 1.0f, 0.0f)));
}

void FlyCamera::enableMouseLock(bool enable)
{
    m_mouseLocked = enable;
    GLFWwindow* window = static_cast<GLFWwindow*>(m_pApp->getWindow());

    if (window)
    {
        if (enable)
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        else
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}