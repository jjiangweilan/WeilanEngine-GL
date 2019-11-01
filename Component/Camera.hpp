#pragma once
#include "Component.hpp"
#include <glm/matrix.hpp>
namespace wlEngine
{
class Camera : public Component
{
public:
    COMPONENT_DECLARATION_ABSTRACT(Component, Camera);
    Camera(Entity *entity);
    virtual glm::mat4 getViewMatrix() const = 0;
    virtual glm::mat4 getProjMatrix() const = 0;
    virtual void setProjectionMatrix(const float &fovy, const float &aspect,
                                     const float &zNear, const float &zFar);
    virtual void setProjectionMatrix(const float &left, const float &right,
                                     const float &bottom, const float &top,
                                     const float &zNear, const float &zFar);
    virtual void update() = 0;

protected:
    glm::mat4 m_projMatrix;
};
} // namespace wlEngine
