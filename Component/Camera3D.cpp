#include "Camera3D.hpp"

#include "../Time.hpp"
#include "../System/RenderSystem.hpp"
#include "../GameEditor/GameEditor.hpp"

#include "../GameObject/Entity.hpp"
#include "../Component/Transform.hpp"
#include "../Input.hpp"
namespace wlEngine
{
COMPONENT_DEFINATION_NEW(Camera, Camera3D);
COMPONENT_EDITABLE_DEF(Camera3D);
Camera3D::Camera3D(Entity *entity) : Camera(entity), front(0, 0, -1), right(1, 0, 0),
                                     speed(10), sensitivity(5)
{
    relX = 0;
    relY = 0;
};
Camera3D::Camera3D(Entity *entity, void **data) : Camera3D(entity)
{
    relX = 0;
    relY = 0;
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

    updateEyeDirection();
    updatePosition();
    if (Input::isMousePressing(MouseButton::Right) && RenderSystem::get()->getGameEditor()->isGameSceneFocused())
    {
        enableMouse = true;
        Input::getMouse(relX, relY);
        Input::enableRelativeMouse(true);
    }
    else
    {
        enableMouse = false;
        Input::enableRelativeMouse(false);
    }
    setProjectionMatrix(45.0f, sceneSize.x / sceneSize.y, 0.1, 100);
}

void Camera3D::updateEyeDirection()
{
    if (enableMouse)
    {
        int currX, currY;
        Input::getMouse(currX, currY);
        relX = currX - relX;
        relY = currY - relY;

        yaw += relX * sensitivity * Time::deltaTime;
		pitch += relY * sensitivity * Time::deltaTime;
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
}
void Camera3D::updatePosition()
{
    auto transform = entity->getComponent<Transform>();
    auto keyboard = SDL_GetKeyboardState(nullptr);
    float speedDelta = speed * Time::deltaTime;

    right = glm::normalize(glm::cross(front, WORLD_UP));
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

	//update y pos
	if (enableMouse) {
		if (Input::isMousePressing(MouseButton::Middle))
		{
			transform->position += (float)relY * glm::cross(right, front) * speedDelta * 0.4f;
		}
	}
}
} // namespace wlEngine
