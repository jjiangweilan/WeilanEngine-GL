#include "VolumetricLight.hpp"
#include "../GameObject/Entity.hpp"
#include "../Graphics/Shader.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, VolumetricLight, 32);
COMPONENT_EDITABLE_DEF(VolumetricLight);
VolumetricLight::VolumetricLight(Entity *entity) : Component(entity)
{
    m_shader = Shader::collection["Basic"];
}
VolumetricLight::VolumetricLight(Entity *, void **data) : Component(entity)
{
    //std::string path = volu
}

const Mesh2D* VolumetricLight::getMesh() const
{
    return &m_mesh;
}
const Shader *VolumetricLight::getShader() const
{
    return m_shader;
}
void VolumetricLight::loadTexture(const std::string &file)
{
    m_mesh.addTexture(file);
}

} // namespace wlEngine