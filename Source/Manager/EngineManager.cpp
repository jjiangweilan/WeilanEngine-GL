#include "Manager/EngineManager.hpp"
#include "System/AnimationSystem.hpp"
#include "System/InputSystem.hpp"
#include "System/NpcSystem.hpp"
#include "System/PhysicsSystem.hpp"
#include "System/RenderSystem.hpp"
#include "System/ScriptSystem.hpp"
#include "System/StateMachineSystem.hpp"
#include "System/LuaScriptSystem.hpp"
#include "System/AudioSystem.hpp"

#include "Component/Script.hpp"
#include "Component/LuaScript.hpp"
#include "Component/RenderScript.hpp"

#include "Graphics/Shader.hpp"
#include "Global/Settings.hpp"

#include "Manager/ResourceManager.hpp"
#include <SDL_mixer.h>

namespace WeilanEngine
{
EngineManager *EngineManager::engine = nullptr;

EngineManager::EngineManager()
{
	quit = false;
	initializeManagers();
	SDLinit();
	initializeSystems();
};

void EngineManager::SDLinit()
{
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0)
	{
		std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
		exit(-1);
	}

	if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) == -1)
	{
		std::cerr << "SDL mixer could not initialize" << std::endl;
		exit(-1);
	}
}

EngineManager::~EngineManager()
{
	SDL_Quit();
};

void EngineManager::postInitialization()
{
	for (auto &script : Script::collection)
	{
		script->PostInit();
	}

	for (auto &luaScript : LuaScript::collection)
	{
		luaScript->PostInit();
	}

	for (auto &renderScript : RenderScript::collection)
	{
		renderScript->PostInit();
	}

	for (auto &system : System::collection)
	{
		system->PostInit();
	}
}

EngineManager *EngineManager::GetWeilanEngine()
{
	if (engine)
		return engine;

	engine = new EngineManager();

	return engine;
}

void EngineManager::SetScene(Scene *scene) { currentScene = scene; }

void EngineManager::loop()
{
	while (!quit)
	{
		if (Settings::engineMode == Settings::EngineMode::Gameplay)
		{
			currentScene->Update();
			systemUpdate();
			Time::Update();
		}
		else
		{
			AnimationSystem::get()->Update();
			currentScene->Update();
			InputSystem::get()->Update();
			RenderSystem::Get()->Update();
			Time::Update();
		}

		currentScene->endFrame();
		if (InputSystem::get()->getKeyStatus(SDL_SCANCODE_F9))
			quit = true;
	}
}

void EngineManager::systemUpdate()
{
	InputSystem::get()->Update();
	PhysicsSystem::get()->Update();
	AnimationSystem::get()->Update();
	NpcSystem::get()->Update();

	//because of the callback, state machine is functionally very close to script system, so it has to be updated right before it
	//eg: some scripts use animation components to decide if a state needs to be ended
	StateMachineSystem::get()->Update();
	LuaScriptSystem::get()->Update();
	ScriptSystem::get()->Update();

	for (auto p : gameplaySystem)
	{
		p->Update();
	}

	AudioSystem::get()->Update();
	RenderSystem::Get()->Update();
}

void EngineManager::initializeSystems()
{
	RenderSystem::init();
	AnimationSystem::init();
	ScriptSystem::init();
	PhysicsSystem::init();
	InputSystem::init();
	StateMachineSystem::init();
	NpcSystem::init();
	LuaScriptSystem::init();
	AudioSystem::init();
}

void EngineManager::initializeManagers() { ResourceManager::init(); }
void EngineManager::addGameplaySystem(System *s) { gameplaySystem.push_back(s); }
void EngineManager::Start()
{
	postInitialization();
	loop();
}
} // namespace WeilanEngine
