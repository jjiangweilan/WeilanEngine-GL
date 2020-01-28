#pragma once
#include "Component.hpp"
#include "../Graphics/Model.hpp"

#include <vector>
#include <functional>
namespace WeilanEngine
{
namespace Graphics
{
class Material;
}
enum DrawMode
{
	Line = GL_LINE,
	Fill = GL_FILL
};
class Model : public Component
{
public:
	COMPONENT_DECLARATION(Component, Model, 32);

	Model(Entity *go, const std::string &id, const bool &unique = false);

	void useShader(const std::string &name);

	const Graphics::Model *GetModel() const;

	void setModel(Graphics::Model *model);
	void SetAllMaterials(const std::string &id);
	void SetAllMaterials(Graphics::Material *mat);
	void MakeUnique();

	const DrawMode &GetDrawMode();
	void SetDrawMode(const DrawMode &mode);

private:
	DrawMode m_drawMode;
	Graphics::Model *m_model;
	std::unique_ptr<Graphics::Model> m_uniqueModel;
};
} // namespace WeilanEngine
