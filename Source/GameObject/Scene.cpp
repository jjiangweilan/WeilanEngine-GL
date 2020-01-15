#include "GameObject/Scene.hpp"

#include "System/System.hpp"
#include "System/PhysicsSystem.hpp"

#include "Utility/Utility.hpp"

#include "Component/Transform.hpp"
#include "Component/Sprite.hpp"
#include "Component/TRigidbody.hpp"
#include "Component/Camera2D.hpp"

#include <memory>
#include <cassert>

namespace WeilanEngine {
void Scene::update() {
}

Scene::Scene() : sceneGraph(), gameObjectAllocator() {}

Scene::~Scene() { clearScene(); }

void Scene::SetMainCamera(Entity *newCamera) { this->camera = newCamera; }

void Scene::reloadScene() {
    Json gameObjects_json = sceneData.data["gameObjects"];
    clearScene();
    sceneData.data = Json::object(); // the structure of data will be built using loadGameObjectFromJson, because createGameObject of SceneData will insert data into sceneData.data
    sceneData.data["scene_path"] = filePath;

    std::map<std::string, Entity *> loadedGameObjects;
    for (auto iter = gameObjects_json.begin(); iter != gameObjects_json.end(); ++iter) {
        loadGameObjectFromFile(iter.value(), iter.key(), gameObjects_json, loadedGameObjects);
    }
}

void Scene::loadScene(const std::string &filePath) {
    clearScene();

    this->filePath = filePath;
    std::ifstream ifs;
    ifs.open(filePath);
    std::ostringstream oss;
    oss << ifs.rdbuf();
    ifs.close();
    sceneData.data = Json::object(); // the structure of data will be built using loadGameObjectFromJson, because createGameObject of SceneData will insert data into sceneData.data
    sceneData.data["scene_path"] = filePath;

    Json gameObjects_json = nlohmann::json::parse(oss.str())["gameObjects"];
    std::map<std::string, Entity *> loadedGameObjects;
    for (auto iter = gameObjects_json.begin(); iter != gameObjects_json.end(); ++iter) {
        loadGameObjectFromFile(iter.value(), iter.key(), gameObjects_json, loadedGameObjects);
    }
}

Entity *Scene::loadGameObjectFromFile(const Json &go_json, const std::string &id, const Json &jsonFile, std::map<std::string, Entity *> &loadedGameObjects) {
    // avoid recreation
    if (loadedGameObjects.find(id) != loadedGameObjects.end())
        return loadedGameObjects[id];

    std::string name = go_json["name"];
    auto &components = go_json["components"];
    auto parent_id = go_json["parent"].get<std::string>();

    // allocate parent gameObject
    Entity *parent = nullptr;
    if (parent_id != Utility::toPointerString(parent))
        parent = loadGameObjectFromFile(jsonFile[parent_id], parent_id, jsonFile, loadedGameObjects);

    // create gameObject with the parent
    auto go = CreateGameObject(name, parent);
    addComponent(go, components);

    sceneData.createGameObject(go, parent, &go_json);
    loadedGameObjects[id] = go;
    return go;
}

void Scene::addComponent(Entity *go, const Json &components) {
    for (auto &iter : components) {
		auto k = Component::getComponentFactoryList();
		auto g = std::hash<std::string>()(iter["name"]);
        auto componentGenerator = (*Component::getComponentFactoryList())[std::hash<std::string>()(iter["name"])];
        assert(componentGenerator && "component is not editable!");

        auto args_json = iter["params"];
        std::vector<void *> args(args_json.size());
        std::vector<std::string> arg_string(args_json.size());
        std::vector<float> arg_float(args_json.size());
        std::vector<Json> arg_unknown(args_json.size());

        int i = 0;
        int argCount_string = 0;
        int argCount_float = 0;
        int argCount_unknown = 0;

        for (nlohmann::json::iterator cIter = args_json.begin(); cIter != args_json.end(); ++cIter) {
            if (cIter->is_string()) {
                arg_string[argCount_string] = cIter->get<std::string>();
                args[i] = &arg_string[argCount_string++];
            } else if (cIter->is_number()) {
                arg_float[argCount_float] = cIter->get<float>();
                args[i] = &arg_float[argCount_float++];
            }  else {
                arg_unknown[argCount_unknown] = *cIter;
                args[i] = &arg_unknown[argCount_unknown++];
            }
            i++;
        }
        componentGenerator(go, args.data());

		if (iter["name"] == "Camera2D" || iter["name"] == "Camera3D") 
		{
			SetMainCamera(go);
		}
    }
}

Entity *Scene::createGameObjectByJson(const Json &go_json, Entity *parent) {
    std::string name = go_json["name"];
    auto &components = go_json["components"];
    auto &children = go_json["children"];

    auto go = CreateGameObject(name, parent);
    addComponent(go, components);

    sceneData.createGameObject(go, parent, &go_json);

    for (Json::const_iterator iter = children.begin(); iter != children.end(); ++iter) {
        CreateGameObject(sceneData.data[iter->get<std::string>()], go);
    }
    return go;
}

void Scene::clearScene() {
    for (auto &k : allocatedGameObjects) {
        gameObjectAllocator.deallocate(k);
    }

    sceneGraph.clear();
    allocatedGameObjects.clear();
    sceneData.clear();

    TRigidbody::staticSpaticalHash.clear();
    TRigidbody::dynamicSpaticalHash.clear();
}

Entity *Scene::CreateGameObject(const std::string &name, Entity *parent) {
    auto ptr = gameObjectAllocator.allocate(name);

    ptr->scene = this;

    if (parent)
        ptr->SetParent(parent);
    else
        addGameObject(ptr);

    allocatedGameObjects.insert(ptr);
    return ptr;
}

void Scene::destroyGameObject(Entity *ptr) {
    entityToBeDeleted.push_back(ptr);
}

void Scene::endFrame() {
    for (auto ptr : entityToBeDeleted) {
        for (auto child : ptr->children) {
            destroyGameObjectHelper(child);
        }
        if (auto parent = ptr->GetParent())
            parent->children.erase(ptr);
        else
            sceneGraph.erase(ptr);
        allocatedGameObjects.erase(ptr);
        gameObjectAllocator.deallocate(ptr);
    }
    entityToBeDeleted.clear();
}

void Scene::destroyGameObjectHelper(Entity *ptr) {
    for (auto child : ptr->children) {
        destroyGameObjectHelper(child);
    }
    allocatedGameObjects.erase(ptr);
    gameObjectAllocator.deallocate(ptr);
}

void Scene::addGameObject(Entity *go) { sceneGraph.insert(go); }

Entity *Scene::findGameObjectNear(const int &mouseX, const int &mouseY) { return findGameObjectNearHelper(sceneGraph.begin(), &sceneGraph, mouseX, mouseY); }

Entity *Scene::findGameObjectNearHelper(std::set<Entity *>::iterator iter, std::set<Entity *> *set, const int &x, const int &y) {
    if (iter != set->end()) {
        auto transform = (*iter)->GetComponent<Transform>();
        auto sprite = (*iter)->GetComponent<Sprite>();
        if (transform && sprite) {
            auto pos = transform->position;
            auto sizeHalf = glm::vec2(sprite->getMesh()->getClipRect()->width, sprite->getMesh()->getClipRect()->height) / 2.0f;
            int minX = pos.x - sizeHalf.x;
            int minY = pos.y - sizeHalf.y;
            int maxX = pos.x + sizeHalf.x;
            int maxY = pos.y + sizeHalf.y;
            if (x >= minX && x <= maxX && y >= minY && y <= maxY) {
                return *iter;
            }
        }
        auto iter_copy = iter;
        if (auto go_next = findGameObjectNearHelper(++iter_copy, set, x, y))
            return go_next;

        auto &children = (*iter)->children;
        for (auto child_iter = children.begin(); child_iter != children.end(); child_iter++) {
            if (auto child_next = findGameObjectNearHelper(child_iter, &children, x, y))
                return child_next;
        }
    }
    return nullptr;
}

Entity *Scene::findGameObjectByName(const std::string &name) { return findGameObjectByNameHelper(sceneGraph.begin(), &sceneGraph, name); }

Entity *Scene::findGameObjectByNameHelper(std::set<Entity *>::iterator iter, std::set<Entity *> *set, const std::string &name) {
    if (iter != set->end()) {
        if ((*iter)->name == name) {
            return *iter;
        }

        auto iter_copy = iter;
        if (auto go_next = findGameObjectByNameHelper(++iter_copy, set, name))
            return go_next;

        auto &children = (*iter)->children;
        for (auto child_iter = children.begin(); child_iter != children.end(); child_iter++) {
            if (auto child_next = findGameObjectByNameHelper(child_iter, &children, name))
                return child_next;
        }
    }
    return nullptr;
}
} // namespace WeilanEngine
