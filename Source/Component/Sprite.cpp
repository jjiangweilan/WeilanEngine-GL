#define STB_IMAGE_IMPLEMENTATION

#include <iostream>

#include "Component/Animation.hpp"
#include "Component/Sprite.hpp"
#include "Component/Camera.hpp"
#include "Manager/EngineManager.hpp"
#include "Global/Settings.hpp"

#include "Manager/ResourceManager.hpp"

namespace WeilanEngine
{
COMPONENT_DEFINATION(Component, Sprite, 64);
COMPONENT_EDITABLE_DEF(Sprite);

Sprite::~Sprite(){};

Sprite::Sprite(Entity *go) : Component(go), transparency(1)
{
    useShader("Sprite");
};

Sprite::Sprite(Entity *go, void **args) : Sprite(go)
{
    if (args)
    {
        std::string path(*static_cast<std::string *>(args[0]));
        std::string shader(*static_cast<std::string *>(args[1]));
        auto resourceManager = ResourceManager::Get();
		if (path.size() != 0)
        {
            m_mesh.addTexture(path);
        }
        useShader(shader);
    }
    else
    {
        useShader("Sprite");
    }
}

Sprite::Sprite(Entity *go, const std::string &path) : Component(go), transparency(1)
{
    auto resourceManager = ResourceManager::Get();
	loadTexture(path);
    useShader("Sprite");
}

void Sprite::loadTexture(const std::string& path) {
	m_mesh.addTexture(path);
}

Sprite::Sprite(Entity *go, const std::string &path, const std::string &shader) : Sprite(go, path) { useShader(shader); }

void Sprite::useShader(const std::string &name) 
{ 
	m_shader = Graphics::Shader::get(name);
}
const Graphics::Shader *Sprite::getShader() const 
{ 
	return m_shader;
}

const Graphics::Mesh2D* Sprite::getMesh() const
{
	return &m_mesh;
}

void Sprite::changeTexture(const int &loc, Graphics::Texture *texture)
{
    m_mesh.changeTexture(loc, texture);
}

Graphics::Texture *Sprite::getMainTexture() const
{
    return m_mesh.getTextures()->at(0);
}
} // namespace WeilanEngine
