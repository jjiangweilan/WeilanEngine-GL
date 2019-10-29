#pragma once
#include "Component.hpp"
#include "../Graphics/Texture.hpp"
namespace wlEngine {
class Shader;
class VolumetricLight : public Component {
    COMPONENT_DECLARATION(Component, VolumetricLight, 32);
    COMPONENT_EDITABLE_DEC();
public:
    VolumetricLight(Entity*);
    VolumetricLight(Entity*, void ** args);
    void loadTexture(const std::string& file);

    public:
    std::vector<Texture> textures;
    Shader* shader;
};

}