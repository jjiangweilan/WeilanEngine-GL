#pragma once
#include "Component/Script.hpp"
#include "Component/Camera.hpp"
namespace WeilanEngine
{
class CameraController : public Script
{
	SCRIPT_DECLARATION(Script, CameraController, 1);

public:
	CameraController(Entity *entity);
	void UpdateEyeDirection();
	void UpdatePosition();
	void Update() override;
	void PostInit() override;

protected:
	Camera *m_attachedCamera;
	int relX;
	int relY;
	float sensitivity;
	bool enableMouse = false;
	float pitch = 0;
	float yaw = -90;

	float speed;
};
} // namespace WeilanEngine