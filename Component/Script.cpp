#include "Script.hpp"

namespace wlEngine {
    const std::size_t Script::componentId = std::hash<std::string>()("Script");
    bool Script::isType(const std::size_t& typeId) const {
        if (typeId == Script::componentId) {
            return true;
        }
        return Component::isType(typeId);
    }
    std::set<Script*> Script::collection = std::set<Script*>();
    size_t Script::getId() {
        return componentId;
    }
}
