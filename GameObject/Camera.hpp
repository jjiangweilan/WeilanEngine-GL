#pragma once
#include "Entity.hpp"
#include "../Settings.hpp"

#include <glm/matrix.hpp>
namespace wlEngine
{
class Camera : public Entity
{
public:
    Camera() : Entity("Camera"){};
    virtual glm::mat4 getViewMatrix() = 0;
    virtual void update() = 0;
};
} // namespace wlEngine
