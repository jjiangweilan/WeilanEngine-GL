#include "Component/LuaScriptBridge.hpp"
#include "System/LuaScriptSystem.hpp"
#include "Manager/EngineManager.hpp"
#include "Component/LuaScript.hpp"
#include "GameObject/Entity.hpp"
namespace WeilanEngine
{
SYSTEM_DEFINATION(LuaScriptSystem);

LuaScriptSystem::LuaScriptSystem()
{
}

void LuaScriptSystem::Update()
{
	auto currentScene = EngineManager::GetWeilanEngine()->GetCurrentScene();
	for (auto &luaScript : LuaScript::collection)
	{
		if (luaScript->GetEntity()->IsEnable() || luaScript->GetEntity()->GetScene() != currentScene)
			lua_getglobal(luaScript->state, "update");
		if (lua_pcall(luaScript->state, 0, 0, 0))
		{
			std::cerr << luaScript->GetEntity()->name << ", error: " << lua_tostring(luaScript->state, -1) << std::endl;
		}
	}
}
} // namespace WeilanEngine