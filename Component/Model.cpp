#include "Model.hpp"
#include "../Manager/ResourceManager.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, Model, 32);

Model::Model(Entity* go) : Component(go), m_model(nullptr), m_drawMode(DrawMode::Fill)
{
    m_shader = Graphics::Shader::get("Model");
}

Model::Model(Entity *go, const std::string &id): Model(go)
{
	m_model = Graphics::Model::get(id);
}

const Graphics::Model* Model::getModel() const
{
    return m_model;
}
const Graphics::Shader *Model::getShader() const
{
    return m_shader;
}

void Model::setModel(Graphics::Model *model)
{
    m_model = model;
}
void Model::setAllMaterials(const std::string& id) {
	for (auto& m : *m_model->getMeshes())
	{
		m.setMaterial(id);
	}
}

void Model::SetDrawMode(const DrawMode &mode)
{
	m_drawMode = mode;
}

const DrawMode &Model::GetDrawMode()
{
	return m_drawMode;
}
} // namespace wlEngine
