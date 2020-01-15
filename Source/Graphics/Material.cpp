#include "Graphics/Material.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Mesh.hpp"
namespace WeilanEngine
{
namespace Graphics
{
std::unordered_map<std::string, Material> Material::collection;
std::set<Material*, Material::MaterialComp> Material::materials;
Material::Material() : m_shader(nullptr), m_attachedMeshes(){};
Material::Material(const unsigned int&renderIndex):Material()
{

}

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

unsigned int Material::AttachToMesh(const Mesh* mesh) const
{
    //TODO: this is not thread safe!
    m_attachedMeshes.push_back(mesh);
    return m_attachedMeshes.size() - 1;
}

void Material::DetachFromMesh(const Mesh *mesh) const
{
    //every mesh must have a material attached to it, so the materialIndex must be valid
    std::swap(m_attachedMeshes.back(), m_attachedMeshes[mesh->m_materialIndex]);
    m_attachedMeshes.pop_back();
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

Material::Material(const Material &other) : Material()
{
    m_parameters = other.m_parameters;
    m_shader = other.m_shader;
	name = other.name;
    m_renderIndex = other.m_renderIndex;

    //the copied material should not contain attached mesh
}
Material::Material(Material &&other)
{
    m_parameters = std::move(other.m_parameters);
	name = std::move(other.name);
    m_shader = other.m_shader;
    m_renderIndex = other.m_renderIndex;
    m_attachedMeshes = std::move(m_attachedMeshes);
}

ShaderParameter *Material::GetParameters() const
{
    return &m_parameters;
}

void Material::SetRenderIndex(const unsigned int& index)
{
    m_renderIndex = index;
}
} // namespace Graphics
} // namespace WeilanEngine
