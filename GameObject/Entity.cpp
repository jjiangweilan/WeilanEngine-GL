#include "Entity.hpp"
#include "../Component/Transform.hpp"
#include "Scene.hpp"
#include "../EngineManager.hpp"
namespace wlEngine {
    Entity::Entity(const std::string& name) : name(name), transform(nullptr), enable(true) { }
	
    void Entity::setParent(Entity* newParent) {
        //remove from previous hierachy
        if (auto preParent = this->parent) preParent->children.erase(this);
        else {
            EngineManager::getwlEngine()->getCurrentScene()->sceneGraph.erase(this);
        }
        
        //add to new hierachy
        if(newParent) {
            this->parent = newParent;
            newParent->children.insert(this);
        }
        else {
            EngineManager::getwlEngine()->getCurrentScene()->sceneGraph.insert(this);
            this->parent = nullptr;
        }

    }

    Entity::~Entity() {
        for(auto& c : components) {
            c->destruct(this);
        }
    }

    void Entity::removeComponent(Component* component) {
		assert(component != transform && "remove transform component is not allowed");
        for(auto c : components) {
            if(c.get() == component) {
                if(c->entities) {
                    c->entities->erase(this);
                }
                components.erase(c);
                return;
            }
        }
    }

	void Entity::setEnable(const bool& enable) {
		this->enable = enable;
		for (auto& child : children) child->setEnable(enable);
	}

	bool Entity::isEnable() {
		return enable;
	}
}
