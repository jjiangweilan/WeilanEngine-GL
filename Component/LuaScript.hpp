#pragma once
#include "Component.hpp"
#include <string>
#include <lua.hpp>
namespace wlEngine
{
class LuaScript : public Component
{
    COMPONENT_DECLARATION(Component, LuaScript, 32);
    LuaScript(Entity *entity, const std::string &file);

    lua_State *state;

    void postInit();

private:
    std::string file;

    static const luaL_Reg gameObjectLib[];
    static int lua_getComponent(lua_State *state);

    static int lua_component_getEntity(lua_State *state);

    static const luaL_Reg transformLib[];
    static int lua_transform_moveBy(lua_State *state);
    static int lua_transform_setPosition(lua_State *state);
    static int lua_transform_setLocalPosition(lua_State *state);
    static int lua_transform_getLocalPosition(lua_State *);
    static int lua_transform_getPosition(lua_State *);

    static const luaL_Reg stateMachineLib[];
    static int lua_stateMachine_addTransition(lua_State *state);
    static int lua_stateMachine_addState(lua_State *state);
    static int lua_stateMachine_setAction(lua_State *state);
    static int lua_stateMachine_changeState(lua_State *state);
    static int lua_stateMachine_getCurrentState(lua_State *state);

    static const luaL_Reg timeLib[];
    static int lua_time_getDeltaTime(lua_State *state);

    static const luaL_Reg TRigidbodyLib[];
    static int lua_tRigidbody_setContactBegin(lua_State *state);
    static int lua_tRigidbody_setContactEnd(lua_State *state);

    static const luaL_Reg animationLib[];
    static int lua_animation_playAnimation(lua_State *state);
    static int lua_animation_addAnimationFromAseprite(lua_State *state);
    static int lua_animation_isPlaying(lua_State *state);
    static int lua_animation_hasEnded(lua_State *state);

    static const luaL_Reg inputLib[];
    static int lua_input_getControllerAxis(lua_State *state);
    static int lua_input_getControllerButton(lua_State *state);
    static int lua_input_isControllerButtonClicked(lua_State *state);
    static int lua_input_isControllerAxisClicked(lua_State *state);
};
} // namespace wlEngine
