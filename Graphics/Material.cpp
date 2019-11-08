#include "Material.hpp"
#include "Shader.hpp"
namespace wlEngine
{

Material::Material() : m_shader(nullptr){};


void Material::setShader(const std::string& name)
{
    m_shader = Shader::collection[name];
}

void Material::useShader() const
{
    m_shader->use();
}

const Shader* Material::getShader()const
{
	return m_shader;
}

} // namespace wlEngine
