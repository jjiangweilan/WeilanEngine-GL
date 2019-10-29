#include "../GameObject/Entity.hpp"
#include "VolumetricLight.hpp"
#include "../Graphics/Shader.hpp"

namespace wlEngine {
    COMPONENT_DEFINATION(Component, VolumetricLight, 32);
    COMPONENT_EDITABLE_DEF(VolumetricLight);
    VolumetricLight::VolumetricLight(Entity* entity) : Component(entity) {
        shader = Shader::collection["Basic"];
    }
    VolumetricLight::VolumetricLight(Entity*, void ** data) : Component(entity) {
    }

    void VolumetricLight::loadTexture(const std::string& file) {
        textures.emplace_back(file);
    }
}