#pragma once
#include "Entity.hpp"
#include <glm/matrix.hpp>
#include <wlEngine/Settings.hpp>
namespace wlEngine {
    class Camera : public Entity {
    public:
        Camera() :Entity("Camera") {};
        virtual glm::mat4 getViewMatrix() = 0;
        virtual void update() = 0;
    };
}
