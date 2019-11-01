#pragma once
#include "Camera.hpp"
#include <glm/matrix.hpp>

#include "../GameObject/Entity.hpp"
#include "../Input.hpp"
#include "../Time.hpp"
namespace wlEngine
{

struct Camera2D : public Camera
{
public:
	COMPONENT_DECLARATION_NEW(Camera, Camera2D);
	COMPONENT_EDITABLE_DEC();

	Camera2D(Entity *go);
	Camera2D(Entity *go, void **);
	glm::mat4 getViewMatrix() const override;
	glm::mat4 getProjMatrix() const override;
	virtual void setProjectionMatrix(const float &left, const float &right,
									 const float &bottom, const float &top, const float &zNear, const float &zFar) override;
	float speed = 100.0;

	void update();
};
} // namespace wlEngine
