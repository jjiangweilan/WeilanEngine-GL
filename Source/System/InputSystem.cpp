#include "System/InputSystem.hpp"
#include "System/RenderSystem.hpp"
#include "Manager/EngineManager.hpp"
#include "Global/Settings.hpp"
#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
namespace WeilanEngine
{
SYSTEM_DEFINATION(InputSystem);

InputSystem::InputSystem() : buttonPressed(SDL_CONTROLLER_BUTTON_MAX, false), 
axisPressed(SDL_CONTROLLER_AXIS_MAX, false), 
gameController(nullptr),
mouseClickedState(5, false)
{
    SDL_GameControllerEventState(SDL_ENABLE);
    if (SDL_NumJoysticks() > 0)
    {
        gameController = SDL_GameControllerOpen(0);
    }
    registerSystem(this);
}

void InputSystem::update()
{
    SDL_Event event;
    reset();
    while (SDL_PollEvent(&event))
    { // "event" is a one time event (won't report key pressing)
        if (Settings::engineMode == Settings::EngineMode::Editor)
            ImGui_ImplSDL2_ProcessEvent(&event);
        switch (event.type)
        {
        case SDL_KEYDOWN:
            keyDown(event);
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseClickedState[SDL_BUTTON(event.button.button)] = true;
            break;
        case SDL_MOUSEWHEEL:
            wheelX = event.wheel.x;
            wheelY = event.wheel.y;
            break;
        }
    }
}

Uint8 InputSystem::getKeyStatus(const SDL_Scancode &scancode) { return SDL_GetKeyboardState(nullptr)[scancode]; }

void InputSystem::keyDown(const SDL_Event &event)
{
    switch (event.key.keysym.scancode)
    {
#if SETTINGS_ENGINEMODE
    case SDL_SCANCODE_F5:
        if (Settings::engineMode == Settings::EngineMode::Gameplay)
        {
            Settings::engineMode = Settings::EngineMode::Editor;
            auto currentScene = EngineManager::GetWeilanEngine()->getCurrentScene();
            currentScene->reloadScene();
        }
        else
        {
            Settings::engineMode = Settings::EngineMode::Gameplay;
        }
        break;
#endif
    case SDL_SCANCODE_F4:
        if (Settings::debugRender == Settings::DebugRender::On)
        {
            Settings::debugRender = Settings::DebugRender::Off;
        }
        else if (Settings::debugRender == Settings::DebugRender::Off)
        {
            Settings::debugRender = Settings::DebugRender::On;
        }
        break;
    default:
        break;
    }
}

void InputSystem::getMouseWheel(int &x, int &y)
{
    x = wheelX;
    y = wheelY;
}

void InputSystem::reset()
{
    wheelY = 0;
    mouseClickedState[0] = false;
    mouseClickedState[1] = false;
    mouseClickedState[2] = false;
    mouseClickedState[3] = false;
    mouseClickedState[4] = false;
}

bool InputSystem::isControllerButtonClicked(const ControllerButton &button)
{
    auto pressed = SDL_GameControllerGetButton(InputSystem::get()->gameController, static_cast<SDL_GameControllerButton>(button));
    auto &buttonLog = buttonPressed[static_cast<int>(button)];
    if (pressed && !buttonLog)
    {
        buttonLog = true;
        return true;
    }
    else if (!pressed && buttonLog)
    {
        buttonLog = false;
    };
    return false;
}

bool InputSystem::isControllerAxisClicked(const ControllerAxis &axis)
{
    auto pressed = SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis));
    auto &axisLog = axisPressed[static_cast<int>(axis)];
    if (pressed && !axisLog)
    {
        axisLog = true;
        return true;
    }
    else if (!pressed && axisLog)
    {
        axisLog = false;
    };
    return false;
}
} // namespace WeilanEngine
