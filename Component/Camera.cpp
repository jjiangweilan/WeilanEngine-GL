#include "Camera.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION_ABSTRACT(Component, Camera);
Camera::Camera(Entity *entity) : Component(entity)
{
};

void Camera::setProjectionMatrix(const float &fovy, const float &aspect, const float &zNear, const float &zFar)
{
    assert(0 && "You are probably setting a 2D camera");
}

void Camera::setProjectionMatrix(const float &left, const float &right,
                                 const float &bottom, const float &top,
                                 const float &zNear, const float &zFar)
{
    assert(0 && "You are probaly settings a 3D camera");
}
} // namespace wlEngine
