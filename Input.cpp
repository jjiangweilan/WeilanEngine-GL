#include "Input.hpp"
#include <iostream>
#include "Settings.hpp"

namespace wlEngine {

int Input::getKeyStatus(SDL_Scancode scancode) {
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        return 0;
    }
#endif
    return InputSystem::get()->getKeyStatus(scancode);
}

void Input::getMouseWheel(int &x, int &y) {
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        x = 0; y = 0; return;
    }
#endif
     return InputSystem::get()->getMouseWheel(x, y); 
     }

ControllerAxisType Input::getControllerAxis(const ControllerAxis &axis) {
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        return 0;
    }
#endif
    if (axis == ControllerAxis::AxisLeftY)
        return -(SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis)) + 1);
    return SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis));
}

ControllerButtonType Input::getControllerButton(const ControllerButton &button) { 
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        return 0;
    }
#endif
    return SDL_GameControllerGetButton(InputSystem::get()->gameController, static_cast<SDL_GameControllerButton>(button)); 
    }

bool Input::isControllerButtonClicked(const ControllerButton &button) { 
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        return false;
    }
#endif
    return InputSystem::get()->isControllerButtonClicked(button); 
    }

bool Input::isControllerAxisClicked(const ControllerAxis &axis) { 
#if SETTINGS_ENGINEMODE
    if (Settings::engineMode == Settings::EngineMode::Editor) {
        return false;
    }
#endif
    return InputSystem::get()->isControllerAxisClicked(axis); 
    }
} // namespace wlEngine
