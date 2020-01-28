#include "Component/Camera.hpp"
#include "GameObject/Entity.hpp"
#include "Graphics/GlobalShaderParameter.hpp"
#include "Manager/EngineManager.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace WeilanEngine
{
COMPONENT_DEFINATION(Component, Camera, 2);
Camera::Camera(Entity *entity) : Component(entity), m_front(0, 0, -1), m_right(1, 0, 0), m_isMainCamera(false), m_renderContext()
{
	bool hasMainCamera = false;
	for (auto camera : Camera::collection)
	{
		if (camera->IsMainCamera())
		{
			hasMainCamera = true;
			break;
		}
	}
	m_isMainCamera = !hasMainCamera;

	m_renderContext.GenFramebuffer();
	m_renderContext.AttachTexture2D(Graphics::PredefinedAttachmentType::Standard);
	m_renderContext.AttachTexture2D(Graphics::PredefinedAttachmentType::Depth);

};

void Camera::SetProjectionMatrix(const float &fovy, const float &aspect, const float &zNear, const float &zFar)
{
	m_projMatrix = glm::perspective(glm::radians(fovy), aspect,
									zNear, zFar);
}

void Camera::SetProjectionMatrix(const float &left, const float &right,
								 const float &bottom, const float &top,
								 const float &zNear, const float &zFar)
{
	m_projMatrix = glm::ortho(left, right, bottom, top, zNear, zFar);
}

void Camera::SetLayerMask(const int32_t &mask)
{
	m_layerMask = mask;
}

const bool &Camera::IsMainCamera() const
{
	return m_isMainCamera;
}

const int32_t &Camera::GetLayerMask() const
{
	return m_layerMask;
}

const int32_t &Camera::GetLayerMask()
{
	return m_layerMask;
}

glm::mat4 Camera::GetViewMatrix() const
{
	auto transform = entity->GetComponent<Transform>();
	return glm::lookAt(transform->position, transform->position + m_front,
					   glm::cross(m_right, m_front));
}

void Camera::SetAsMainCamera()
{
	for (auto &camera : Camera::collection)
	{
		camera->m_isMainCamera = false;
	}
	m_isMainCamera = true;
}
const glm::mat4 &Camera::GetProjMatrix() const
{
	return m_projMatrix;
}

void Camera::SetFront(const glm::vec3 &newFront)
{
	m_front = newFront;
}
void Camera::SetRight(const glm::vec3 &newRight)
{
	m_right = newRight;
}
void Camera::SetWorldUp(const glm::vec3 &newWorldUp)
{
	m_worldUp = newWorldUp;
}
const glm::vec3 &Camera::GetFront() const
{
	return m_front;
}
const glm::vec3 &Camera::GetRight() const
{
	return m_right;
}
const glm::vec3 &Camera::GetWorldUp() const
{
	return m_worldUp;
}
Graphics::RenderContext *Camera::GetRenderContext()
{
	return &m_renderContext;
}

Camera* Camera::GetMainCamera()
{
	auto currentScene = EngineManager::GetWeilanEngine()->GetCurrentScene();
	for (auto c : collection)
	{
		if (c->IsMainCamera() && c->GetEntity()->GetScene() == currentScene) return c;
	}
}
} // namespace WeilanEngine
