#include "Input.hpp"
#include <iostream>
#include "Settings.hpp"

namespace wlEngine {

int Input::getKeyStatus(SDL_Scancode scancode) {
    return InputSystem::get()->getKeyStatus(scancode);
}

void Input::getMouseWheel(int &x, int &y) {
     return InputSystem::get()->getMouseWheel(x, y); 
     }

ControllerAxisType Input::getControllerAxis(const ControllerAxis &axis) {
    if (axis == ControllerAxis::AxisLeftY)
        return -(SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis)) + 1);
    return SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis));
}

ControllerButtonType Input::getControllerButton(const ControllerButton &button) { 
    return SDL_GameControllerGetButton(InputSystem::get()->gameController, static_cast<SDL_GameControllerButton>(button)); 
    }

bool Input::isControllerButtonClicked(const ControllerButton &button) { 
    return InputSystem::get()->isControllerButtonClicked(button); 
    }

bool Input::isControllerAxisClicked(const ControllerAxis &axis) { 
// #if SETTINGS_ENGINEMODE && DEBUG
//     if (Settings::engineMode == Settings::EngineMode::Editor) {
//         return false;
//     }
// #endif
    return InputSystem::get()->isControllerAxisClicked(axis); 
    }
} // namespace wlEngine
