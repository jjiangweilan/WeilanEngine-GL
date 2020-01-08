#include "Component/Model.hpp"
#include "Manager/ResourceManager.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, Model, 32);

Model::Model(Entity * entity, const std::string &id, const bool& copy): Component(entity), m_drawMode(DrawMode::Fill)
{
	if(!copy)m_model = Graphics::Model::Get(id);
	else
	{
		m_model = nullptr;
		m_uniqueModel = std::make_unique<Graphics::Model>(*Graphics::Model::Get(id));
	}
}


void Model::MakeUnique()
{
	if (!m_uniqueModel && m_model)
	{
		m_uniqueModel = std::make_unique<Graphics::Model>(*m_model);
		m_model = nullptr;
	}
	else 
	{
		std::cerr << "Model, MakeUnique failed" << std::endl;
	}
}

const Graphics::Model* Model::GetModel() const
{
	if(m_model) return m_model;
	else return m_uniqueModel.get();
}

void Model::setModel(Graphics::Model *model)
{
    m_model = model;
}
void Model::SetAllMaterials(const std::string& id) {
	for (auto& m : *m_model->GetMeshes())
	{
		m.SetMaterial(id);
	}
}

void Model::SetAllMaterials(Graphics::Material* mat)
{
	for (auto& m : *m_model->GetMeshes())
	{
		m.SetMaterial(mat);
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
