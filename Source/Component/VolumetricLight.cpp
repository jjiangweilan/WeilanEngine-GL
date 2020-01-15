#include "Component/VolumetricLight.hpp"
#include "GameObject/Entity.hpp"
#include "Graphics/Shader.hpp"

namespace WeilanEngine
{
COMPONENT_DEFINATION(Component, VolumetricLight, 32);
COMPONENT_EDITABLE_DEF(VolumetricLight);
VolumetricLight::VolumetricLight(Entity *entity) : Component(entity)
{
    m_shader = Graphics::Shader::get("Basic");
}
VolumetricLight::VolumetricLight(Entity *, void **data) : Component(entity)
{
    //std::string path = volu
}

const Graphics::Mesh2D* VolumetricLight::getMesh() const
{
    return &m_mesh;
}
const Graphics::Shader *VolumetricLight::getShader() const
{
    return m_shader;
}
void VolumetricLight::loadTexture(const std::string &file)
{
    m_mesh.addTexture(file);
}

} // namespace WeilanEngine