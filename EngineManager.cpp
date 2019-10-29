#include "EngineManager.hpp"
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

#include "Graphics/Shader.hpp"
#include "Settings.hpp"

#include "Manager/ResourceManager.hpp"
#include <SDL_mixer.h>

namespace wlEngine {
EngineManager *EngineManager::engine = nullptr;

EngineManager::EngineManager() {
    quit = false;
    SDLinit();
    initializeSystems();
    initializeManagers();
};

void EngineManager::SDLinit() {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_GAMECONTROLLER) < 0) {
        std::cerr << "SDL could not initialize! SDL Error: " << SDL_GetError() << std::endl;
        exit(-1);
    }

    if (Mix_OpenAudio(22050, MIX_DEFAULT_FORMAT, 8, 4096) == -1) {
        std::cerr << "SDL mixer could not initialize" << std::endl;
        exit(-1);
    }
}

EngineManager::~EngineManager(){
    SDL_Quit();
};

void EngineManager::postInitialization() {
    for (auto &script : Script::collection) {
        script->postInit();
    }

    for (auto &luaScript : LuaScript::collection) {
        luaScript->postInit();
    }
}

EngineManager *EngineManager::getwlEngine() {
    if (engine)
        return engine;

    engine = new EngineManager();
    return engine;
}

void EngineManager::setScene(Scene *scene) { currentScene = scene; }

void EngineManager::loop() {
#if SETTINGS_ENGINEMODE
    while (!quit) {
        if (Settings::engineMode == Settings::EngineMode::Gameplay) {
        currentScene->update();
            systemUpdate();
            Time::update();
        } else {
            AnimationSystem::get()->update();
            if (currentScene->camera)
                RenderSystem::get()->update();
            InputSystem::get()->update();
            currentScene->update();
            Time::update();
        }

        currentScene->endFrame();
    }
#else
    while (!quit) {
        systemUpdate();
        currentScene->update();
        Time::update();
		currentScene->destroyGameObjectEndOfLoop();
    }
#endif

    
}

void EngineManager::systemUpdate() {
    InputSystem::get()->update();
    PhysicsSystem::get()->update();
    AnimationSystem::get()->update();
    NpcSystem::get()->update();

    //because of the callback, state machine is functionally very close to script system, so it has to be updated right before it
    //eg: some scripts use animation components to decide if a state needs to be ended
    StateMachineSystem::get()->update();
    LuaScriptSystem::get()->update();
    ScriptSystem::get()->update();

    for (auto p : gameplaySystem) {
        p->update();
    }

    AudioSystem::get()->update();
    RenderSystem::get()->update();
}

void EngineManager::initializeSystems() {
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
} // namespace wlEngine
