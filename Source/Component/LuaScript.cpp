#include "Component/LuaScript.hpp"
#include "Component/LuaScriptBridge.hpp"

#include "GameObject/Entity.hpp"
#include "Global/Time.hpp"
#include "Global/Input.hpp"
#include "Component/Transform.hpp"
#include "Component/StateMachine.hpp"
#include "Component/TRigidbody.hpp"
#include "Component/Animation.hpp"
#include <string>

//Defination for LuaScriptBridge
//The implementation is moved here because the sizes of the arrays need to be known prior to using them
namespace WeilanEngine
{
const luaL_Reg LuaScriptBridge::transformLib[] = {
	COMPONENT_LIB,
	{"moveBy", LuaScriptBridge::lua_transform_moveBy},
	{"setPosition", LuaScriptBridge::lua_transform_setPosition},
	{"setLocalPosition", LuaScriptBridge::lua_transform_setLocalPosition},
	{"getPosition", LuaScriptBridge::lua_transform_getPosition},
	{"getLocalPosition", LuaScriptBridge::lua_transform_getLocalPosition},
	{nullptr, nullptr}};

const luaL_Reg LuaScriptBridge::stateMachineLib[] = {
	COMPONENT_LIB,
	{"addTransition", LuaScriptBridge::lua_stateMachine_addTransition},
	{"setPosition", LuaScriptBridge::lua_stateMachine_addTransition},
	{"setAction", LuaScriptBridge::lua_stateMachine_setAction},
	{"changeState", LuaScriptBridge::lua_stateMachine_changeState},
	{"getCurrentState", LuaScriptBridge::lua_stateMachine_getCurrentState},
	{nullptr, nullptr}};
const luaL_Reg LuaScriptBridge::TRigidbodyLib[] = {
	COMPONENT_LIB,
	{"setContactBegin", LuaScriptBridge::lua_tRigidbody_setContactBegin},
	{"setContactEnd", LuaScriptBridge::lua_tRigidbody_setContactEnd},
	{nullptr, nullptr}};
const luaL_Reg LuaScriptBridge::animationLib[] = {
	COMPONENT_LIB,
	{"playAnimation", LuaScriptBridge::lua_animation_playAnimation},
	{"addAnimationFromAseprite", LuaScriptBridge::lua_animation_addAnimationFromAseprite},
	{"isPlaying", LuaScriptBridge::lua_animation_isPlaying},
	{"hasEnded", LuaScriptBridge::lua_animation_hasEnded},
	{nullptr, nullptr}};
const luaL_Reg LuaScriptBridge::timeLib[] = {
	{"getDeltaTime", LuaScriptBridge::lua_time_getDeltaTime},
	{nullptr, nullptr}};
const luaL_Reg LuaScriptBridge::inputLib[] = {
	{"getControllerAxis", LuaScriptBridge::lua_input_getControllerAxis},
	{"getControllerButton", LuaScriptBridge::lua_input_getControllerButton},
	{"isControllerButtonClicked", LuaScriptBridge::lua_input_isControllerButtonClicked},
	{"isControllerAxisClicked", LuaScriptBridge::lua_input_isControllerAxisClicked},
	{nullptr, nullptr}};
const luaL_Reg LuaScriptBridge::gameObjectLib[] = {{"getComponent", LuaScriptBridge::lua_getComponent}, {nullptr, nullptr}};

/* Component*/
int LuaScriptBridge::lua_component_getEntity(lua_State *state)
{
	auto component = *static_cast<Component **>(lua_touserdata(state, 1));
	Entity **userData = static_cast<Entity **>(lua_newuserdata(state, sizeof(Entity *)));
	*userData = component->GetEntity();

	/* the meta table*/
	lua_newtable(state);
	lua_pushstring(state, "__index");
	luaL_newlib(state, gameObjectLib);
	lua_settable(state, 3);
	lua_setmetatable(state, 2); // table out
	return 1;
}

int LuaScriptBridge::lua_getComponent(lua_State *state)
{
	auto entity = *static_cast<Entity **>(lua_touserdata(state, 1));
	size_t componentId = std::hash<std::string>()(lua_tostring(state, 2));

	for (auto &c : entity->components)
	{
		if (c->isType(componentId))
		{
			Component **userData = static_cast<Component **>(lua_newuserdata(state, sizeof(void *)));
			*userData = &*c;
			if (componentId == Transform::componentId)
			{
				LUA_CREATE_COMPONENT(transformLib);
			}
			else if (componentId == StateMachine::componentId)
			{
				LUA_CREATE_COMPONENT(stateMachineLib);
			}
			else if (componentId == TRigidbody::componentId)
			{
				LUA_CREATE_COMPONENT(TRigidbodyLib);
			}
			else if (componentId == Animation::componentId)
			{
				LUA_CREATE_COMPONENT(animationLib);
			}
			return 1;
		}
	}
	lua_pushnil(state);
	return 1;
}
/*Transform*/
int LuaScriptBridge::lua_transform_moveBy(lua_State *state)
{
	auto transform = *static_cast<Transform **>(lua_touserdata(state, 1));
	float x = lua_tonumber(state, 2);
	float y = lua_tonumber(state, 3);
	float z = lua_tonumber(state, 4);

	transform->moveBy(x, y, z);
	return 0;
}

int LuaScriptBridge::lua_transform_setPosition(lua_State *state)
{
	auto transform = *static_cast<Transform **>(lua_touserdata(state, -4));
	float x = lua_tonumber(state, -3);
	float y = lua_tonumber(state, -2);
	float z = lua_tonumber(state, -1);

	transform->setPosition({x, y, z});
	return 0;
}
int LuaScriptBridge::lua_transform_setLocalPosition(lua_State *state)
{
	auto transform = *static_cast<Transform **>(lua_touserdata(state, 1));
	float x = lua_tonumber(state, 2);
	float y = lua_tonumber(state, 3);
	float z = lua_tonumber(state, 4);

	transform->setLocalPosition({x, y, z});
	return 0;
}
int LuaScriptBridge::lua_transform_getLocalPosition(lua_State *state)
{
	auto transform = *static_cast<Transform **>(lua_touserdata(state, 1));

	auto pos = transform->getLocalPosition();
	lua_newtable(state); // 2
	lua_pushstring(state, "x");
	lua_pushnumber(state, pos.x);
	lua_settable(state, 2);
	lua_pushstring(state, "y");
	lua_pushnumber(state, pos.y);
	lua_settable(state, 2);
	lua_pushstring(state, "z");
	lua_pushnumber(state, pos.z);
	lua_settable(state, 2);

	return 1;
}
int LuaScriptBridge::lua_transform_getPosition(lua_State *state)
{
	auto transform = *static_cast<Transform **>(lua_touserdata(state, 1));

	auto pos = transform->position;
	lua_newtable(state); // 2
	lua_pushstring(state, "x");
	lua_pushnumber(state, pos.x);
	lua_settable(state, 2);
	lua_pushstring(state, "y");
	lua_pushnumber(state, pos.y);
	lua_settable(state, 2);
	lua_pushstring(state, "z");
	lua_pushnumber(state, pos.z);
	lua_settable(state, 2);

	return 1;
}

/* StateMachine*/
int LuaScriptBridge::lua_stateMachine_addTransition(lua_State *state)
{
	auto stateMachine = *static_cast<StateMachine **>(lua_touserdata(state, 1));

	assert(typeid(StateType) == typeid(std::string));
	StateType from = lua_tostring(state, 2);
	StateType to = lua_tostring(state, 3);

	static std::map<StateType, std::map<StateType, int>> refLog;
	int ref;
	auto hasRef = refLog[from].find(to);
	if (hasRef == refLog[from].end())
	{
		ref = luaL_ref(state, LUA_REGISTRYINDEX);
		refLog[from][to] = ref;
	}
	else
	{
		ref = hasRef->second;
		lua_rawseti(state, LUA_REGISTRYINDEX, ref); //set the new func
	}

	stateMachine->addTransition(from, to, [=]() -> StatePriority {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
		lua_call(state, 0, 1);
		StatePriority rlt = lua_tonumber(state, -1);
		lua_pop(state, 1);
		return rlt;
	});

	return 0;
}
int LuaScriptBridge::lua_stateMachine_addState(lua_State *state)
{
	auto stateMachine = *static_cast<StateMachine **>(lua_touserdata(state, 1));
	const char *name = lua_tostring(state, 2);

	stateMachine->addState(name);
	return 0;
}
int LuaScriptBridge::lua_stateMachine_setAction(lua_State *state)
{
	auto stateMachine = *static_cast<StateMachine **>(lua_touserdata(state, 1));
	std::string stateStr = lua_tostring(state, 2);
	std::string stateActionIndexStr = lua_tostring(state, 3);

	int stateActionIndex = 0;
	if (stateActionIndexStr[1] == 'n')
		stateActionIndex = StateActionEnterFuncIndex; // enter
	else if (stateActionIndexStr[0] == 'u')
		stateActionIndex = StateActionUpdateFuncIndex; // update
	else if (stateActionIndexStr[0] == 'e')
		stateActionIndex = StateActionExitFuncIndex; //exit

	static std::map<std::string, std::map<std::string, int>> refLog;
	int ref;
	auto hasRef = refLog[stateStr].find(stateActionIndexStr);
	if (hasRef == refLog[stateStr].end())
	{
		ref = luaL_ref(state, LUA_REGISTRYINDEX);
		refLog[stateStr][stateActionIndexStr] = ref;
	}
	else
	{
		ref = hasRef->second;
		lua_rawseti(state, LUA_REGISTRYINDEX, ref);
	}

	(*(stateMachine->getActionGroup(stateStr)))[stateActionIndex] = [=]() {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);
		lua_call(state, 0, 0);
	};

	return 0;
}
int LuaScriptBridge::lua_stateMachine_changeState(lua_State *state)
{
	auto stateMachine = *static_cast<StateMachine **>(lua_touserdata(state, 1));
	const char *newState = lua_tostring(state, 2);

	stateMachine->changeState(newState);
	return 0;
}
int LuaScriptBridge::lua_stateMachine_getCurrentState(lua_State *state)
{
	auto stateMachine = *static_cast<StateMachine **>(lua_touserdata(state, 1));
	lua_pushstring(state, stateMachine->getCurrentState().data());
	return 1;
}

/* Time*/
int LuaScriptBridge::lua_time_getDeltaTime(lua_State *state)
{
	lua_pushnumber(state, Time::deltaTime);
	return 1;
}

/* Input*/
int LuaScriptBridge::lua_input_getControllerAxis(lua_State *state)
{
	int axis = lua_tointeger(state, 1);
	int rlt = Input::getControllerAxis(static_cast<ControllerAxis>(axis));
	lua_pushinteger(state, rlt);
	return 1;
}
int LuaScriptBridge::lua_input_getControllerButton(lua_State *state)
{
	int button = lua_tointeger(state, 1);
	int rlt = Input::getControllerButton(static_cast<ControllerButton>(button));
	lua_pushinteger(state, rlt);
	return 1;
}
int LuaScriptBridge::lua_input_isControllerButtonClicked(lua_State *state)
{
	int button = lua_tointeger(state, 1);
	bool rlt = Input::isControllerButtonClicked(static_cast<ControllerButton>(button));
	lua_pushboolean(state, rlt);
	return 1;
}
int LuaScriptBridge::lua_input_isControllerAxisClicked(lua_State *state)
{
	int axis = lua_tointeger(state, 1);
	bool rlt = Input::isControllerAxisClicked(static_cast<ControllerAxis>(axis));
	lua_pushboolean(state, rlt);
	return 1;
}

/* TRgidbody*/
int LuaScriptBridge::lua_tRigidbody_setContactBegin(lua_State *state)
{
	auto body = *static_cast<TRigidbody **>(lua_touserdata(state, 1));

	static int ref = -1;
	if (ref == -1)
	{
		ref = luaL_ref(state, LUA_REGISTRYINDEX);
	}
	else
	{
		lua_rawseti(state, LUA_REGISTRYINDEX, ref);
	}

	body->contactBegin = [=](TRigidbody *body, TRigidbody *other) {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

		TRigidbody **bodyData = static_cast<TRigidbody **>(lua_newuserdata(state, sizeof(void *)));
		*bodyData = body;
		LUA_CREATE_COMPONENT(LuaScriptBridge::TRigidbodyLib);

		TRigidbody **otherData = static_cast<TRigidbody **>(lua_newuserdata(state, sizeof(void *)));
		*otherData = other;
		LUA_CREATE_COMPONENT(LuaScriptBridge::TRigidbodyLib);

		lua_call(state, 2, 0);
	};

	return 0;
}
int LuaScriptBridge::lua_tRigidbody_setContactEnd(lua_State *state)
{
	auto body = *static_cast<TRigidbody **>(lua_touserdata(state, 1));

	static int ref = -1;
	if (ref == -1)
	{
		ref = luaL_ref(state, LUA_REGISTRYINDEX);
	}
	else
	{
		lua_rawseti(state, LUA_REGISTRYINDEX, ref);
	}

	body->contactEnd = [=](TRigidbody *body, TRigidbody *other) {
		lua_rawgeti(state, LUA_REGISTRYINDEX, ref);

		TRigidbody **bodyData = static_cast<TRigidbody **>(lua_newuserdata(state, sizeof(void *)));
		*bodyData = body;
		LUA_CREATE_COMPONENT(LuaScriptBridge::TRigidbodyLib);

		TRigidbody **otherData = static_cast<TRigidbody **>(lua_newuserdata(state, sizeof(void *)));
		*otherData = other;
		LUA_CREATE_COMPONENT(LuaScriptBridge::TRigidbodyLib);

		lua_call(state, 2, 0);
	};

	return 0;
}

/* animation*/
int LuaScriptBridge::lua_animation_playAnimation(lua_State *state)
{
	auto animation = *static_cast<Animation **>(lua_touserdata(state, 1));
	const char *name = lua_tostring(state, 2);

	animation->playAnimation(name);
	return 0;
}
int LuaScriptBridge::lua_animation_addAnimationFromAseprite(lua_State *state)
{
	auto animation = *static_cast<Animation **>(lua_touserdata(state, 1));
	const char *json = lua_tostring(state, 2);
	const char *file = lua_tostring(state, 3);

	animation->addAnimationFromAseprite(json, file);
	return 0;
}
int LuaScriptBridge::lua_animation_isPlaying(lua_State *state)
{
	auto animation = *static_cast<Animation **>(lua_touserdata(state, 1));
	const char *name = lua_tostring(state, 2);

	bool rlt = animation->isPlaying(name);
	lua_pushboolean(state, rlt);
	return 1;
}
int LuaScriptBridge::lua_animation_hasEnded(lua_State *state)
{
	auto animation = *static_cast<Animation **>(lua_touserdata(state, 1));
	bool rlt = animation->hasEnded();
	lua_pushboolean(state, rlt);
	return 1;
}
} // namespace WeilanEngine

namespace WeilanEngine
{
COMPONENT_DEFINATION(Component, LuaScript, 32);

LuaScript::LuaScript(Entity *entity, const std::string &file) : Component(entity), file(file)
{
	state = luaL_newstate();
	luaL_requiref(state, "_G", luaopen_base, 1);
	lua_pop(state, 1);
	Entity **userData = static_cast<Entity **>(lua_newuserdata(state, sizeof(Entity *)));
	*userData = entity;

	/* the meta table*/
	lua_newtable(state); // 2
	lua_pushstring(state, "__index");
	luaL_newlib(state, LuaScriptBridge::gameObjectLib);
	lua_settable(state, 2);
	lua_setmetatable(state, 1); // table out
	lua_setglobal(state, "entity");

	/* Gameplay Lib*/
	LUA_CREATE_LIB("Time", timeLib);
	LUA_CREATE_LIB("Input", inputLib);
	lua_getglobal(state, "Input");
	LUA_SET_FIELD("ButtonLeft", SDL_CONTROLLER_BUTTON_X);
	LUA_SET_FIELD("ButtonRight", SDL_CONTROLLER_BUTTON_B);
	LUA_SET_FIELD("ButtonUp", SDL_CONTROLLER_BUTTON_Y);
	LUA_SET_FIELD("ButtonDown", SDL_CONTROLLER_BUTTON_A);
	LUA_SET_FIELD("LeftShoulder", SDL_CONTROLLER_BUTTON_LEFTSHOULDER);
	LUA_SET_FIELD("RightShoulder", SDL_CONTROLLER_BUTTON_RIGHTSHOULDER);
	LUA_SET_FIELD("DpadDown", SDL_CONTROLLER_BUTTON_DPAD_DOWN);
	LUA_SET_FIELD("DpadUp", SDL_CONTROLLER_BUTTON_DPAD_UP);
	LUA_SET_FIELD("DpadLeft", SDL_CONTROLLER_BUTTON_DPAD_LEFT);
	LUA_SET_FIELD("DpadRight", SDL_CONTROLLER_BUTTON_DPAD_RIGHT);
	LUA_SET_FIELD("Start", SDL_CONTROLLER_BUTTON_START);
	LUA_SET_FIELD("Back", SDL_CONTROLLER_BUTTON_BACK);
	LUA_SET_FIELD("RightStick", SDL_CONTROLLER_BUTTON_RIGHTSTICK);
	LUA_SET_FIELD("LeftStick", SDL_CONTROLLER_BUTTON_LEFTSTICK);
	LUA_SET_FIELD("Home", SDL_CONTROLLER_BUTTON_GUIDE);
	LUA_SET_FIELD("AxisLeftX", SDL_CONTROLLER_AXIS_LEFTX);
	LUA_SET_FIELD("AxisLeftY", SDL_CONTROLLER_AXIS_LEFTY);
	LUA_SET_FIELD("AxisRightX", SDL_CONTROLLER_AXIS_RIGHTX);
	LUA_SET_FIELD("AxisRightY", SDL_CONTROLLER_AXIS_RIGHTY);
	LUA_SET_FIELD("TriggerLeft", SDL_CONTROLLER_AXIS_TRIGGERLEFT);
	LUA_SET_FIELD("TriggerRight", SDL_CONTROLLER_AXIS_TRIGGERRIGHT);
	LUA_SET_FIELD("AxisMax", 0x7fff);
	lua_pop(state, 1);
}
void LuaScript::PostInit()
{
	if (luaL_dofile(state, file.data()))
	{
		std::cerr << "load lua file failed: " << file << std::endl;
		std::cout << lua_tostring(state, -1);
	}
}
} // namespace WeilanEngine
