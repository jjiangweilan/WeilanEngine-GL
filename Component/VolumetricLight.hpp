#pragma once
#include "Component.hpp"
#include "../Graphics/Mesh2D.hpp"
namespace wlEngine
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

    const Mesh2D* getMesh() const;
    const Shader *getShader() const;

private:
    Mesh2D m_mesh;
    Shader *m_shader;
};

} // namespace wlEngine