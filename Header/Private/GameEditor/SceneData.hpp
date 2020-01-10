#pragma once
#include "Global/Settings.hpp"
#include "GameObject/Entity.hpp"

#include <map>
#include <json.hpp>

namespace KuangyeEngine
{
using Json = nlohmann::json;
enum class EditableComponent
{
    Sprite,
    Transform
};
class SceneData
{
public:
    Json data;
    void createGameObject(Entity *, Entity *, const Json *);
    void clear();
    void destroyGameObject(Entity *);
    void removeComponent(Entity *, const std::string &);

    //json is an array of json object
    void addComponent(Entity *, const Json &);
    void changeHierachy(Entity *child, Entity *parent);
    void editTransform(Entity *, const int &x, const int &y, const int &z);
    Json &getData(Entity *);

private:
    void eraseGameObjectFromChildren(const std::string &parent, const std::string &child);
};
} // namespace KuangyeEngine
