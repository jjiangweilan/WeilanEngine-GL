#pragma once
#include <wlEngine/Component/Component.hpp>
#include <wlEngine/Component/Transform.hpp>
#include <wlEngine/Settings.hpp>
#include <memory>

namespace wlEngine {
    class Scene;
    class Entity {
    public:
        Entity(const std::string& id);
        ~Entity();
        std::string name;
		std::set<Entity*> children;
		
        Entity* getParent() {return parent;}
        void setParent(Entity*);

        std::set<std::shared_ptr<Component>> components;
        
        template <typename ComponentType, typename... Args>
		ComponentType* addComponent(Args&& ... params);

        template <typename ComponentType>
        void addComponent(std::shared_ptr<ComponentType>& );
        
        template <typename ComponentType>
        void removeComponent();

        template <typename ComponentType>
        ComponentType* getComponent();

        void removeComponent(Component* c);

		template<>
		Transform* getComponent<Transform>();

		Scene* getScene() { return scene; }

		void setEnable(const bool& enable);
		bool isEnable();
    private:
        Entity* parent = nullptr;
		Scene* scene = nullptr;
		Transform* transform;
		bool enable;
        friend struct Transform;
        friend class SceneData;
		friend class Scene;
    };

	template<>
	Transform* Entity::getComponent<Transform>() {
		if(transform) return transform;
		for (auto& c : components) {
			if (c->isType(Transform::componentId)) {
				transform = static_cast<Transform*>(c.get());
				return transform;
			}
		}

		return nullptr;
	}

    template<typename ComponentType, typename... Args>
	ComponentType* Entity::addComponent(Args&&... params) {
		auto p = ComponentType::createComponent(this, params...);
		auto raw = p.get();
        components.insert(p);
		return raw;
    }

    template <typename ComponentType>
    ComponentType* Entity::getComponent() {
        for (auto& c : components) {
            if (c->isType(ComponentType::componentId)) {
                return static_cast<ComponentType*>(c.get());
            }
            
        }

        return nullptr;
    }

    template <typename ComponentType>
    void Entity::addComponent(std::shared_ptr<ComponentType>& c) {
        if (c->gameObjects == nullptr) c->gameObject = this;
        else c->gameObjects.insert(this);

        components.insert(c);
    }

    template <typename ComponentType>
        void Entity::removeComponent() {
            for (auto& c : components) {
                if (c->isType(ComponentType::componentId)) {
                    if (c->entities) {
                        c->entities->erase(this);
                    }
					assert(c.get() != transform && "remove transform component is not allowed");
                    components.erase(c);
                    return;
                }
            }
        }
}
