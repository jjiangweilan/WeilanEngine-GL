#pragma once
#include "Component.hpp"
#include "../Graphics/Model.hpp"

#include <vector>
#include <functional>
namespace wlEngine
{
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

private:
	Graphics::Model* m_model;
    Graphics::Shader *m_shader;
};
} // namespace wlEngine
