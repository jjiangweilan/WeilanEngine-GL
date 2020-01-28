#pragma once
#include "Component.hpp"
#include "Graphics/RenderContext.hpp"
#include <glm/matrix.hpp>
namespace WeilanEngine
{
class Camera : public Component
{
public:
	COMPONENT_DECLARATION(Component, Camera, 2);
	Camera(Entity *entity);
	virtual glm::mat4 GetViewMatrix() const;
	virtual const glm::mat4 &GetProjMatrix() const;
	virtual void SetProjectionMatrix(const float &fovy, const float &aspect,
									 const float &zNear, const float &zFar);
	virtual void SetProjectionMatrix(const float &left, const float &right,
									 const float &bottom, const float &top,
									 const float &zNear, const float &zFar);

	void SetLayerMask(const int32_t &mask);
	void SetFront(const glm::vec3 &newFront);
	void SetRight(const glm::vec3 &newFront);
	void SetWorldUp(const glm::vec3 &newWorldUp);
	void SetAsMainCamera();
	const glm::vec3 &GetFront() const;
	const glm::vec3 &GetRight() const;
	const glm::vec3 &GetWorldUp() const;
	const int32_t &GetLayerMask() const;
	const int32_t &GetLayerMask();
	Graphics::RenderContext *GetRenderContext();
	const bool &IsMainCamera() const;

	static Camera* GetMainCamera();
protected:
	int32_t m_layerMask;
	glm::mat4 m_projMatrix;

	glm::vec3 m_front;
	glm::vec3 m_right;
	glm::vec3 m_worldUp = glm::vec3(0, 1, 0);
	bool m_isMainCamera;
	Graphics::RenderContext m_renderContext;

	friend class RenderSystem;
};
} // namespace WeilanEngine
