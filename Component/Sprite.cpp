#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include "Animation.hpp"
#include "Sprite.hpp"
#include "../GameObject/Camera.hpp"
#include "../EngineManager.hpp"
#include "../Settings.hpp"

#include "../Manager/ResourceManager.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION(Component, Sprite, 100);
COMPONENT_EDITABLE_DEF(Sprite);

Sprite::~Sprite(){};

Sprite::Sprite(Entity *go) : Component(go), transparency(1)
{
    useShader("Sprite");
};

Sprite::Sprite(Entity *go, void **args) : Component(go), transparency(1)
{
    if (args)
    {
        std::string path(*static_cast<std::string *>(args[0]));
        std::string shader(*static_cast<std::string *>(args[1]));
        auto resourceManager = ResourceManager::get();
        m_mainTexture = resourceManager->getTexture(path);
        useShader(shader);
    }
    else
    {
        useShader("Sprite");
    }
}

Sprite::Sprite(Entity *go, const std::string &path) : Component(go), transparency(1)
{
    auto resourceManager = ResourceManager::get();
    m_mainTexture = resourceManager->getTexture(path);
    useShader("Sprite");
}

Sprite::Sprite(Entity *go, const std::string &path, const std::string &shader) : Sprite(go, path) { useShader(shader); }

void Sprite::useShader(const std::string &name) { m_shader = Shader::collection[name]; }

void Sprite::addTexture(const std::string &id, const std::string &path) { m_textures[id] = ResourceManager::get()->getTexture(path); }

void Sprite::removeTexture(const std::string &id) { m_textures.erase(id); }
void Sprite::setMainTexture(Texture *t)
{
    m_mainTexture = t;
}

const std::map<std::string, Texture *> Sprite::getTextures() const { return m_textures; }
const Texture *Sprite::getMainTexture() const { return m_mainTexture; }
const Shader *Sprite::getShader() const { return m_shader; }
} // namespace wlEngine
