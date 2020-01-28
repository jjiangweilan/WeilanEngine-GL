#include "System/ScriptSystem.hpp"
#include "Manager/EngineManager.hpp"
#include "Component/Script.hpp"
#include "GameObject/Entity.hpp"
namespace WeilanEngine
{
ScriptSystem *ScriptSystem::system = nullptr;

ScriptSystem::ScriptSystem()
{
	registerSystem(this);
}

void ScriptSystem::Update()
{
	auto currentScene = EngineManager::GetWeilanEngine()->GetCurrentScene();
	for (auto &s : Script::collection)
	{
		if (!s->GetEntity()->IsEnable() || s->GetEntity()->GetScene() != currentScene)
			continue;
		s->Update();
	}
}
void ScriptSystem::PostInit()
{
	for (auto &s : Script::collection)
	{
		s->PostInit();
	}
}

void ScriptSystem::init()
{
	system = new ScriptSystem();
}
} // namespace WeilanEngine
