#include "Component/RenderScript.hpp"
namespace wlEngine
{
const std::size_t RenderScript::componentId = std::hash<std::string>()("RenderScript");
bool RenderScript::isType(const std::size_t &typeId) const
{
    if (typeId == RenderScript::componentId)
    {
        return true;
    }
    return Component::isType(typeId);
}
std::set<RenderScript *> RenderScript::collection = std::set<RenderScript *>();
size_t RenderScript::getId()
{
    return componentId;
}
} // namespace wlEngine

