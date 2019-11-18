#pragma once
#include "Component.hpp"
#include "../Graphics/Model.hpp"

#include <vector>
#include <functional>
namespace wlEngine
{

enum DrawMode {
    Line = GL_LINE, Fill = GL_FILL
};
class Model : public Component
{
    COMPONENT_DECLARATION(Component, Model, 32);

public:
    Model(Entity *go);
    Model(Entity *go, const std::string &id);

    void useShader(const std::string &name);

    std::function<void()> beforeRenderFunc;
    std::function<void()> afterRenderFunc;
    
    const Graphics::Model * getModel() const;
    const Graphics::Shader* getShader() const;

    void setModel(Graphics::Model* model);
	void setAllMaterials(const std::string& id);

    const DrawMode& GetDrawMode();

    void SetDrawMode(const DrawMode& mode);
private:
    DrawMode m_drawMode;
	Graphics::Model* m_model;
    Graphics::Shader *m_shader;
};
} // namespace wlEngine
