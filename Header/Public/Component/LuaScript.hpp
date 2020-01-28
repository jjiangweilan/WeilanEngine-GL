#pragma once
#include "Component.hpp"
#include <string>
class lua_State;
namespace WeilanEngine
{
class LuaScript : public Component
{
	COMPONENT_DECLARATION(Component, LuaScript, 32);
	LuaScript(Entity *entity, const std::string &file);

	lua_State *state;

	void PostInit();

private:
	std::string file;
};
} // namespace WeilanEngine
