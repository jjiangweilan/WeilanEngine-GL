#pragma once
#include "Component.hpp"
#include <glm/matrix.hpp>
#include "Transform.hpp"
#include "../Input.hpp"
#include "../Time.hpp"
namespace wlEngine
{
struct CoordTransform
{
	glm::mat4 view;
	glm::mat4 projection; //
};

struct Camera2D : Component
{
	COMPONENT_DECLARATION(Component, Camera2D, 3);
	COMPONENT_EDITABLE_DEC();

public:
	Camera2D(Entity *go);
	Camera2D(Entity *go, void **) : Camera2D(go){};
	const CoordTransform &getTransformMatrix();
	float speed = 100.0;
	Transform *transform = nullptr;

	void update()
	{
		const int DEAD_ZONE = 10000;
		ControllerAxisType xx, yy;
		xx = Input::getControllerAxis(ControllerAxis::AxisRightX);
		yy = -(Input::getControllerAxis(ControllerAxis::AxisRightY) + 1); // this calculation is done to prevent integer overflow

		int radius2 = xx * xx + yy * yy;
		if (radius2 < 100000000)
		{
			xx = 0;
			yy = 0;
		}
		float x = (float)xx / Input::AXIS_MAX;
		float y = (float)yy / Input::AXIS_MAX;
		float left = Input::getKeyStatus(SDL_SCANCODE_LEFT) - x;
		float right = Input::getKeyStatus(SDL_SCANCODE_RIGHT) + x;
		float up = Input::getKeyStatus(SDL_SCANCODE_UP) + y;
		float down = Input::getKeyStatus(SDL_SCANCODE_DOWN) - y;
		transform->moveBy(Time::deltaTime * 3000 * (-left + right), Time::deltaTime * 3000 * (up - down), 0);
	}

private:
	CoordTransform coordTransform;
	glm::vec2 viewport;
	float viewportScale;
};
} // namespace wlEngine
