#include "Component/Component.hpp"

namespace WeilanEngine
{
std::map<std::size_t, std::string> *Component::componentIdToName =
    getComponentIdToName();
std::map<std::size_t, std::function<void(Entity *, void **)>>
    *Component::componentFactoryList = getComponentFactoryList();
Component::Component(Entity *entity) : entity(entity), enable(true)
{
}
const std::size_t Component::componentId =
    std::hash<std::string>()("Component");
bool Component::isType(const std::size_t &typeId) const
{
    return Component::componentId == typeId;
}

std::map<std::size_t, std::function<void(Entity *, void **)>> *
Component::getComponentFactoryList()
{
    if (componentFactoryList == nullptr)
    {
        componentFactoryList =
            new std::map<std::size_t, std::function<void(Entity *, void **)>>();
    }

    return componentFactoryList;
}

std::size_t Component::getId() { return componentId; }

std::size_t Component::genComponentId(const std::string &c)
{
    std::size_t id = std::hash<std::string>()(c);
    (*getComponentIdToName())[id] = c;
    return id;
}

std::map<std::size_t, std::string> *Component::getComponentIdToName()
{
    if (Component::componentIdToName == nullptr)
    {
        Component::componentIdToName = new std::map<std::size_t, std::string>;
    }
    return Component::componentIdToName;
}
} // namespace WeilanEngine
