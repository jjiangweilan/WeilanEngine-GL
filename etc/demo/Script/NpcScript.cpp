#include "NpcScript.hpp"

#include <string>
SCRIPT_DEFINATION(Script, NpcScript, 32);

NpcScript::NpcScript(Entity* entity) : Script(entity) {
	std::string name = entity->name;

}

void NpcScript::update() {}

