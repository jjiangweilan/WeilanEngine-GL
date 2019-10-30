#pragma once
#include <wlEngine/wlEngine.hpp>
#include "ContactFilterDefine.hpp"
using namespace wlEngine;

struct ItemScript : public Script, Interactable {
    SCRIPT_DECLARATION(Script, ItemScript, 32);
    ItemScript(Entity* go) : Script(go), Interactable(InteractableType::Collectable){
    }    

    void contactBeginCallback(b2Fixture* self, b2Fixture* other) {
    }

    void update() override{
	
	};
};
