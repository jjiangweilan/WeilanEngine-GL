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

const Graphics::Model* Model::getModel() const
{
    return m_model;
}
const Shader *Model::getShader() const
{
    return m_shader;
}

void Model::setModel(Graphics::Model *model)
{
    m_model = model;
}
} // namespace wlEngine
