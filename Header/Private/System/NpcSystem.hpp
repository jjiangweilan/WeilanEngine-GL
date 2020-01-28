#pragma once

#include "System.hpp"

namespace WeilanEngine
{
class NpcController;
class NpcSystem : public System
{
public:
	static NpcSystem *get() { return system; }
	static void init();
	void Update() override;

private:
	NpcSystem();
	static NpcSystem *system;

	void move(NpcController *npcController);
	void trade(NpcController *npcController);
};
} // namespace WeilanEngine