#pragma once
#include "../Component/Component.hpp"
#include "../Component/Transform.hpp"
#include "../Settings.hpp"
#include <memory>

namespace wlEngine {
    class Scene;
    class Entity {
    public:
        Entity(const std::string& id);
        ~Entity();
        std::string name;
		std::set<Entity*> children;
		
        Entity* GetParent() {return parent;}
        void SetParent(Entity*);

        std::set<std::shared_ptr<Component>> components;
        
        template <typename ComponentType, typename... Args>
		ComponentType* AddComponent(Args&& ... params);

        template <typename ComponentType>
        void AddComponent(std::shared_ptr<ComponentType>& );
        
        template <typename ComponentType>
        void removeComponent();

        template <typename ComponentType>
        ComponentType* GetComponent();

        void RemoveComponent(Component* c);

		template<>
		Transform* GetComponent<Transform>();

		Scene* GetScene() { return scene; }

		void SetEnable(const bool& enable);
		bool IsEnable();
    private:
        Entity* parent = nullptr;
		Scene* scene = nullptr;
		Transform* transform;
		bool enable;
        int32_t m_layer;
        friend struct Transform;
        friend class SceneData;
		friend class Scene;
    };

	template<>
	Transform* Entity::GetComponent<Transform>() {
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
	ComponentType* Entity::AddComponent(Args&&... params) {
		auto p = ComponentType::createComponent(this, std::forward<Args>(params)...);
		auto raw = p.get();
        components.insert(p);
		return raw;
    }

    template <typename ComponentType>
    ComponentType* Entity::GetComponent() {
        for (auto& c : components) {
            if (c->isType(ComponentType::componentId)) {
                return static_cast<ComponentType*>(c.get());
            }
            
        }

        return nullptr;
    }

    template <typename ComponentType>
    void Entity::AddComponent(std::shared_ptr<ComponentType>& c) {
        if (c->gameObjects == nullptr) c->gameObject = this;
        else c->gameObjects.insert(this);

        components.insert(c);
    }

    template <typename ComponentType>
        void Entity::removeComponent() {
            for (auto& c : components) {
                if (c->isType(ComponentType::componentId)) {
					assert(c.get() != transform && "remove transform component is not allowed");
                    components.erase(c);
                    return;
                }
            }
        }
}
