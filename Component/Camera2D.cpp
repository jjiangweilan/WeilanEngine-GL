#include "Camera2D.hpp"
#include "../GameObject/Entity.hpp"
#include "../System/RenderSystem.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, Camera2D, 3);
COMPONENT_EDITABLE_DEF(Camera2D);

Camera2D::Camera2D(Entity *go) : Component(go)
{
    viewport = RenderSystem::get()->getSceneSize();
    viewportScale = viewport.x / viewport.y;
    transform = go->getComponent<Transform>();
}

const CoordTransform &Camera2D::getTransformMatrix()
{
    //#if SETTINGS_ENGINEMODE

    //#endif
    coordTransform.view = glm::lookAt(transform->position, transform->position + glm::vec3(0, 0, -1), glm::vec3(0, 1, 0));
    coordTransform.projection = glm::ortho(0.0f, (float)viewport.x, 0.0f, (float)viewport.y, -1000000.0f, 1000000.0f);
    return coordTransform;
}

} // namespace wlEngine
