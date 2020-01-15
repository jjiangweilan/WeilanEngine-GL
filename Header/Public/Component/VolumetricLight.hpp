#pragma once
#include "Component.hpp"
#include "../Graphics/Mesh2D.hpp"
namespace WeilanEngine
{
class Shader;
class VolumetricLight : public Component
{
    COMPONENT_DECLARATION(Component, VolumetricLight, 32);
    COMPONENT_EDITABLE_DEC();

public:
    VolumetricLight(Entity *);
    VolumetricLight(Entity *, void **args);
    void loadTexture(const std::string &file);

    const Graphics::Mesh2D* getMesh() const;
    const Graphics::Shader *getShader() const;

private:
    Graphics::Mesh2D m_mesh;
    Graphics::Shader *m_shader;
};

} // namespace WeilanEngine