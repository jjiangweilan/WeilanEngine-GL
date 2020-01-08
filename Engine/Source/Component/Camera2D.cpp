#include "Component/Camera2D.hpp"
#include "GameObject/Entity.hpp"
#include "System/RenderSystem.hpp"

namespace wlEngine
{

COMPONENT_DEFINATION_NEW(Camera, Camera2D);
COMPONENT_EDITABLE_DEF(Camera2D);


Camera2D::Camera2D(Entity *go) : Camera(go)
{
}

Camera2D::Camera2D(Entity *go, void **data) : Camera(go)
{
}

glm::mat4 Camera2D::GetViewMatrix() const
{
	auto transform = entity->GetComponent<Transform>();
    return glm::lookAt(transform->position, transform->position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
}

glm::mat4 Camera2D::GetProjMatrix() const
{
    return m_projMatrix;
}

void Camera2D::SetProjectionMatrix(const float &left, const float &right,
                                   const float &bottom, const float &top, const float &zNear, const float &zFar)
{
    m_projMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera2D::update()
	{
		auto transform = entity->GetComponent<Transform>();
		const int DEAD_ZONE = 10000;
		ControllerAxisType xx, yy;
		xx = Input::getControllerAxis(ControllerAxis::AxisRightX);
		yy = Input::getControllerAxis(ControllerAxis::AxisRightY); 

		int radius2 = xx * xx + yy * yy;
		if (radius2 < 100000000)
		{
			xx = 0;
			yy = 0;
		}

		float x = (float)xx / Input::AXIS_MAX;
		float y = (float)yy / Input::AXIS_MAX;
		float left = Input::getKeyStatus(SDL_SCANCODE_J) - x;
		float right = Input::getKeyStatus(SDL_SCANCODE_L) + x;
		float up = Input::getKeyStatus(SDL_SCANCODE_I) + y;
		float down = Input::getKeyStatus(SDL_SCANCODE_K) - y;
		transform->moveBy(Time::deltaTime * 3000 * (-left + right), Time::deltaTime * 3000 * (up - down), 0);
        
        auto size = RenderSystem::Get()->GetSceneSize();
        SetProjectionMatrix(0, size.x, 0, size.y, -100000,100000);
	}
} // namespace wlEngine
