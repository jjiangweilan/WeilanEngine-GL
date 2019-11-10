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
}

void Material::useShader() const
{
    m_shader->use();
}

const Shader *Material::getShader() const
{
    return m_shader;
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

} // namespace Graphics
} // namespace wlEngine
