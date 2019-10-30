#include "ScriptSystem.hpp"
#include "../EngineManager.hpp"
#include "../Component/Script.hpp"
#include "../GameObject/Entity.hpp"
namespace wlEngine
{
ScriptSystem *ScriptSystem::system = nullptr;

ScriptSystem::ScriptSystem()
{
    registerSystem(this);
}

void ScriptSystem::update()
{
    auto currentScene = EngineManager::getwlEngine()->getCurrentScene();
    for (auto &s : Script::collection)
    {
        if (!s->entity->isEnable() || s->entity->getScene() != currentScene)
            continue;
        s->update();
    }
}
void ScriptSystem::postInit()
{
    for (auto &s : Script::collection)
    {
        s->postInit();
    }
}

void ScriptSystem::init()
{
    system = new ScriptSystem();
}
} // namespace wlEngine
