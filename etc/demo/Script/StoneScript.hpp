#pragma once
#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;

struct StoneScript : public Script {
	SCRIPT_DECLARATION(Script, StoneScript, 2);
	COMPONENT_EDITABLE_DEC();

    StoneScript(Entity* go) : Script(go) {}
};

COMPONENT_EDITABLE_DEF(StoneScript);
