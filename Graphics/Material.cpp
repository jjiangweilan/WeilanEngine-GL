#include "Material.hpp"
#include "Shader.hpp"
namespace wlEngine
{
namespace Graphics
{
std::unordered_map<std::string, Material> Material::collection;
Material::Material() : m_shader(nullptr){};

void Material::setShader(const std::string &name)
{
    m_shader = Shader::get(name);
    m_parameters.UpdateParameters(m_shader);
}

void Material::useShader() const
{
    m_shader->use();
}

const Shader *Material::getShader() const
{
    return m_shader;
}

Material Material::Clone() const
{
    return *this;
}

Material *Material::get(const std::string &id)
{
    auto iter = collection.find(id);
    if (iter == collection.end()) return nullptr;
    return &iter->second;
}

void Material::remove(const std::string &id)
{
    collection.erase(id);
}

Material::Material(const Material &other)
{
    m_parameters = other.m_parameters;
    m_textures = other.m_textures;
    m_shader = other.m_shader;
}
Material::Material(Material &&other)
{
    m_parameters = std::move(other.m_parameters);
    m_textures = std::move(other.m_textures);
    m_shader = other.m_shader;
}

ShaderParameter* Material::GetParameters() const
{
    return &m_parameters;
}
} // namespace Graphics
} // namespace wlEngine
