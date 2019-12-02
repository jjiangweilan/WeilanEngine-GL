#include "Material.hpp"
#include "Shader.hpp"
namespace wlEngine
{
namespace Graphics
{
std::unordered_map<std::string, Material> Material::collection;
Material::Material() : m_shader(nullptr){};

void Material::SetShader(const std::string &name)
{
    m_shader = Shader::get(name);
    m_parameters.UpdateParameters(m_shader);
}

void Material::UseMaterial() const
{
    m_shader->Use();
    m_parameters.Use();
}

const Shader *Material::GetShader() const
{
    return m_shader;
}
void Material::ReloadShader() const
{
    m_shader->Reload();
    m_parameters.UpdateParameters(m_shader);
}

Material Material::Clone() const
{
    return *this;
}

Material &Material::operator=(const Material & other)
{
    m_parameters = other.m_parameters;
    m_shader = other.m_shader;
	name = other.name;
    return *this;
}
Material &Material::operator=(Material && other)
{
    m_parameters = std::move(other.m_parameters);
    m_shader = other.m_shader;
	name = std::move(other.name);
    return *this;
}

Material *Material::Get(const std::string &id)
{
    auto iter = collection.find(id);
    if (iter == collection.end())
        return nullptr;
    return &iter->second;
}

void Material::remove(const std::string &id)
{
    collection.erase(id);
}

Material::Material(const Material &other)
{
    m_parameters = other.m_parameters;
    m_shader = other.m_shader;
	name = other.name;
}
Material::Material(Material &&other)
{
    m_parameters = std::move(other.m_parameters);
	name = std::move(other.name);
    m_shader = other.m_shader;
}

ShaderParameter *Material::GetParameters() const
{
    return &m_parameters;
}
} // namespace Graphics
} // namespace wlEngine
