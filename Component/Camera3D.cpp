#include "Camera3D.hpp"

#include "../Time.hpp"
#include "../System/RenderSystem.hpp"

#include "../GameObject/Entity.hpp"
#include "../Component/Transform.hpp"
namespace wlEngine
{
COMPONENT_DEFINATION_NEW(Camera, Camera3D);
COMPONENT_EDITABLE_DEF(Camera3D);
Camera3D::Camera3D(Entity* entity) : Camera(entity), front(0, 0, -1), right(1, 0, 0)
{
    SDL_GetMouseState(&relX, &relY);
};
Camera3D::Camera3D(Entity *entity, void **data): Camera(entity), front(0, 0, -1), right(1, 0, 0)
{
}
Camera3D::~Camera3D(){};

glm::mat4 Camera3D::getViewMatrix() const
{
    auto transform = entity->getComponent<Transform>();
    return glm::lookAt(transform->position, transform->position + front,
                       glm::cross(right, front));
}

glm::mat4 Camera3D::getProjMatrix() const
{
    return m_projMatrix;
}

void Camera3D::setProjectionMatrix(const float &fovy, const float &aspect,
                                   const float &zNear, const float &zFar)
{

    m_projMatrix = glm::perspective(glm::radians(fovy), aspect,
                                    zNear, zFar);
}

void Camera3D::update()
{
	auto sceneSize = RenderSystem::get()->getSceneSize();
	setProjectionMatrix(glm::radians(45.0f), sceneSize.x / sceneSize.y, 0.1, 100000);
    updatePosition();
    updateEyeDirection();
    if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT))
    {
        SDL_SetRelativeMouseMode(SDL_TRUE);
        enableMouse = true;
    }
    else
    {
        SDL_SetRelativeMouseMode(SDL_FALSE);
        enableMouse = false;
    }
}
void Camera3D::updateEyeDirection()
{
    if (enableMouse)
    {
        SDL_GetRelativeMouseState(&relX, &relY);
        yaw += relX * sensitivity;
        pitch -= relY * sensitivity;
    }

    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    float yawRadians = glm::radians(yaw);
    float pitchRadians = glm::radians(pitch);
    front.x = glm::cos(yawRadians) * glm::cos(pitchRadians);
    front.y = glm::sin(pitchRadians);
    front.z = glm::sin(yawRadians) * glm::cos(pitchRadians);
    front = glm::normalize(front);
}
void Camera3D::updatePosition()
{
    auto transform = entity->getComponent<Transform>();
    auto keyboard = SDL_GetKeyboardState(nullptr);
    right = glm::normalize(glm::cross(front, WORLD_UP));

    float speedDelta = speed * Time::deltaTime;
    if (keyboard[SDL_SCANCODE_LEFT])
    {
        transform->position -= right * speedDelta;
    }
    if (keyboard[SDL_SCANCODE_RIGHT])
    {
        transform->position += right * speedDelta;
    }
    if (keyboard[SDL_SCANCODE_UP])
    {
        transform->position += front * speedDelta;
    }
    if (keyboard[SDL_SCANCODE_DOWN])
    {
        transform->position -= front * speedDelta;
    }
}
} // namespace wlEngine
