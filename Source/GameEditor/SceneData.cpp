#include "GameEditor/SceneData.hpp"
#include "Utility/Utility.hpp"

#include <sstream>

namespace KuangyeEngine
{

void SceneData::createGameObject(Entity *go, Entity *parent, const Json *json_ptr)
{
	nlohmann::json j;

	if (json_ptr == nullptr)
	{
		j["name"] = go->name;
		j["components"] = Json::array();
		j["children"] = Json::array();
		json_ptr = &j;
	}

	std::string goId = Utility::toPointerString(go);
	data["gameObjects"][goId] = *json_ptr;
	data["gameObjects"][goId]["children"] = Json::array();

	std::string parentId = Utility::toPointerString(parent);
	data["gameObjects"][goId]["parent"] = parentId;
	if (parent)
		data["gameObjects"][parentId]["children"].push_back(goId);
}

void SceneData::clear()
{
	data.clear();
}

void SceneData::destroyGameObject(Entity *go)
{
	std::string goId = Utility::toPointerString(go);

	if (go->parent)
	{
		std::string parent = Utility::toPointerString(go->parent);
		auto &children = data["gameObjects"][parent]["children"];
		for (auto iter = children.begin(); iter != children.end(); ++iter)
		{
			if (*iter == goId)
			{
				children.erase(iter - children.begin());
				break;
			}
		}
	}

	auto children = data["gameObjects"][goId]["children"];
	for (auto iter = children.begin(); iter != children.end(); ++iter)
	{
		data["gameObjects"].erase(iter->get<std::string>());
	}

	data["gameObjects"].erase(goId);
}

Json &SceneData::getData(Entity *go)
{
	return data["gameObjects"][Utility::toPointerString(go)];
}

void SceneData::removeComponent(Entity *go, const std::string &c)
{
	std::string goId = Utility::toPointerString(go);
	auto &components = data["gameObjects"][goId]["components"];
	for (auto iter = components.begin(); iter != components.end(); ++iter)
	{
		if ((*iter)["name"] == c)
		{
			components.erase(iter);
			break;
		}
	}
}
void SceneData::eraseGameObjectFromChildren(const std::string &parent, const std::string &child)
{
	auto &children = data["gameObjects"][parent]["children"];
	for (auto iter = children.begin(); iter != children.end(); ++iter)
	{
		if (*iter == child)
		{
			children.erase(iter);
			break;
		}
	}
}

void SceneData::addComponent(Entity *go, const Json &components)
{
	auto goId = Utility::toPointerString(go);
	for (auto &iter : components)
	{
		data["gameObjects"][goId]["components"].push_back(iter);
	}
}

void SceneData::changeHierachy(Entity *parent, Entity *child)
{
	auto parentId = Utility::toPointerString(parent);
	auto childId = Utility::toPointerString(child);
	if (child->parent)
	{
		eraseGameObjectFromChildren(Utility::toPointerString(child->parent), childId);
	}

	data["gameObjects"][childId]["parent"] = parentId;
	data["gameObjects"][parentId]["children"].push_back(childId);
}

void SceneData::editTransform(Entity *go, const int &x, const int &y, const int &z)
{
	auto goId = Utility::toPointerString(go);
	auto goJson = data["gameObjects"].find(goId);
	if (goJson != data["gameObjects"].end())
	{
		auto t = Utility::findComponentWithName(*goJson, "Transform");
		if (t)
		{
			auto &j = *t;
			j["params"][0] = x;
			j["params"][1] = y;
			j["params"][2] = z;
		}
	}
}
} // namespace KuangyeEngine
