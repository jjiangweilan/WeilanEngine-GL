#pragma once
#include "Component.hpp"
#include <glm/matrix.hpp>
namespace wlEngine
{
class Camera : public Component
{
public:
    COMPONENT_DECLARATION(Component, Camera, 2);
    Camera(Entity *entity);
    virtual glm::mat4 GetViewMatrix() const = 0;
    virtual glm::mat4 GetProjMatrix() const = 0;
    virtual void SetProjectionMatrix(const float &fovy, const float &aspect,
                                     const float &zNear, const float &zFar);
    virtual void SetProjectionMatrix(const float &left, const float &right,
                                     const float &bottom, const float &top,
                                     const float &zNear, const float &zFar);
    virtual void update() = 0;

    void SetLayerMask(const int32_t& mask);
    const int32_t& GetLayerMask() const;

    const int32_t& GetLayerMask();
protected:
    int32_t m_layerMask;
    glm::mat4 m_projMatrix;
};
} // namespace wlEngine
