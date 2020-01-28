#include "GameObject/Entity.hpp"
#include "GameObject/Scene.hpp"
#include "Component/Transform.hpp"
#include "Manager/EngineManager.hpp"
namespace WeilanEngine
{
Entity::Entity(const std::string &name) : name(name), transform(nullptr), enable(true) {}

void Entity::SetParent(Entity *newParent)
{
	//remove from previous hierachy
	if (auto preParent = this->parent)
		preParent->children.erase(this);
	else
	{
		this->scene->sceneGraph.erase(this);
	}

	//add to new hierachy
	if (newParent)
	{
		this->parent = newParent;
		newParent->children.insert(this);
	}
	else
	{
		this->scene->sceneGraph.insert(this);
		this->parent = nullptr;
	}
}

Entity::~Entity()
{
	for (auto &c : components)
	{
		c->destruct(this);
	}
}

void Entity::RemoveComponent(Component *component)
{
	assert(component != transform && "remove transform component is not allowed");
	for (auto c : components)
	{
		if (c.get() == component)
		{
			components.erase(c);
			return;
		}
	}
}

void Entity::SetEnable(const bool &enable)
{
	this->enable = enable;
	for (auto &child : children)
		child->SetEnable(enable);
}

bool Entity::IsEnable()
{
	return enable;
}
} // namespace WeilanEngine
