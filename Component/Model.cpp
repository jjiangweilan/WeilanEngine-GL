#include "Model.hpp"
#include "../Manager/ResourceManager.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, Model, 32);

Model::Model(Entity* go) : Component(go), m_model(nullptr)
{
    m_shader = Shader::collection["Model"];
}

Model::Model(Entity *go, const std::string &path): Model(go)
{
    m_model = ResourceManager::get()->getModel(path);
}

const Model3D *Model::getModel3D() const
{
    return m_model;
}
const Shader *Model::getShader() const
{
    return m_shader;
}
} // namespace wlEngine
