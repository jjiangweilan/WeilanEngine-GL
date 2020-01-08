#include "System/ScriptSystem.hpp"
#include "Manager/EngineManager.hpp"
#include "Component/Script.hpp"
#include "GameObject/Entity.hpp"
namespace wlEngine
{
ScriptSystem *ScriptSystem::system = nullptr;

ScriptSystem::ScriptSystem()
{
    registerSystem(this);
}

void ScriptSystem::update()
{
    auto currentScene = EngineManager::GetwlEngine()->getCurrentScene();
    for (auto &s : Script::collection)
    {
        if (!s->entity->IsEnable() || s->entity->GetScene() != currentScene)
            continue;
        s->update();
    }
}
void ScriptSystem::PostInit()
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
