#include "Material.hpp"
#include "Shader.hpp"
namespace wlEngine
{

Material::Material() : m_shader(nullptr){};

Material::Material(const std::string& shader, std::vector<Texture*>&& textures){
    setShader(shader);
    changeTextures(std::move(textures));
};

void Material::setShader(const std::string& name)
{
    m_shader = Shader::collection[name];
}

void Material::useShader() const
{
    m_shader->use();
}

} // namespace wlEngine
