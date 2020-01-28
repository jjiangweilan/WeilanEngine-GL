#include "Component/CameraController.hpp"
#include "Component/Transform.hpp"

#include "System/RenderSystem.hpp"
#include "GameEditor/GameEditor.hpp"
#include "GameObject/Entity.hpp"

#include "Global/Time.hpp"
#include "Global/Input.hpp"
namespace WeilanEngine
{
SCRIPT_DEFINATION(Script, CameraController, 1);

CameraController::CameraController(Entity *entity) : Script(entity), speed(10), sensitivity(5)
{
}

void CameraController::Update()
{
	auto sceneSize = RenderSystem::Get()->GetSceneSize();

	UpdateEyeDirection();
	UpdatePosition();
	if (Input::isMousePressing(MouseButton::Right) && GameEditor::get()->isGameSceneFocused())
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
	m_attachedCamera->SetProjectionMatrix(45.0f, sceneSize.x / sceneSize.y, 0.1, 100);
}

void CameraController::UpdateEyeDirection()
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

		auto front = m_attachedCamera->GetFront();
		front.x = glm::cos(yawRadians) * glm::cos(pitchRadians);
		front.y = glm::sin(pitchRadians);
		front.z = glm::sin(yawRadians) * glm::cos(pitchRadians);
		front = glm::normalize(front);
		m_attachedCamera->SetFront(front);
	}
}
void CameraController::UpdatePosition()
{
	auto transform = entity->GetComponent<Transform>();
	auto keyboard = SDL_GetKeyboardState(nullptr);
	float speedDelta = speed * Time::deltaTime;

	auto right = m_attachedCamera->GetRight();
	auto front = m_attachedCamera->GetFront();
	auto WORLD_UP = m_attachedCamera->GetWorldUp();
	right = glm::normalize(glm::cross(front, WORLD_UP));
	m_attachedCamera->SetRight(right);
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
	if (enableMouse)
	{
		if (Input::isMousePressing(MouseButton::Middle))
		{
			transform->position += (float)relY * glm::cross(right, front) * speedDelta * 0.4f;
		}
	}
}

void CameraController::PostInit()
{
	m_attachedCamera = entity->GetComponent<Camera>();
}
} // namespace WeilanEngine