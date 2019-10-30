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
}

void VolumetricLight::loadTexture(const std::string &file)
{
    m_textures.emplace_back(file);
}
const std::vector<Texture> &VolumetricLight::getTextures() const
{
    return m_textures;
}
const Shader *VolumetricLight::getShader() const
{
    return m_shader;
}

} // namespace wlEngine