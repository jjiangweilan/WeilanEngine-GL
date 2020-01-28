#pragma once
#include "System.hpp"
namespace WeilanEngine
{
class ScriptSystem : public System
{
public:
	void Update() override;
	void PostInit();

	static void init();
	static ScriptSystem *get() { return system; };

private:
	static ScriptSystem *system;
	ScriptSystem();
};
} // namespace WeilanEngine