#pragma once
#include "../wolf/wolf.h"
#include "../samplefw/Sample.h"
#include "../samplefw/OrbitCamera.h"

class CityBuildings : public Sample {
public:
    CityBuildings(wolf::App* pApp);
    ~CityBuildings();

    void init() override;
    void update(float dt) override;
    void render(int width, int height) override;

    GLuint getProgram() const { return m_program; }

    // Global controls setters
    void setSpacing(float s) { m_spacing = s; }
    void setBuildingHeightRange(float minH, float maxH) { m_buildingScaleMin = minH; m_buildingScaleMax = maxH; }
    void setInstanceCount(int count) { m_instanceCount = count; }

private:
    void createCubeGeometry();

    GLuint m_program = 0;
    GLuint m_vbo = 0;
    GLuint m_indexBuffer = 0;
    GLuint m_vao = 0;

    OrbitCamera* m_pOrbitCam = nullptr;

    // Cached uniform locations
    GLint m_uViewProjLoc = -1;
    GLint m_uSpacingLoc = -1;
    GLint m_uMinHeightLoc = -1;
    GLint m_uMaxHeightLoc = -1;

    // Global control values
    float m_spacing = 7.0f;
    float m_buildingScaleMin = 2.0f;
    float m_buildingScaleMax = 12.0f;
    int m_instanceCount = 400;
};