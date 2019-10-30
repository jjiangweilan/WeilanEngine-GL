#pragma once
#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;

class NpcScript : public Script {
    SCRIPT_DECLARATION(Script, NpcScript, 32);
    NpcScript(Entity* entity);
    void update() override;
};
