#include "Global/Input.hpp"
#include "Global/Settings.hpp"
#include <iostream>
#ifdef DEBUG
#include "System/RenderSystem.hpp"
#include "GameEditor/GameEditor.hpp"
#endif

namespace KuangyeEngine
{
int Input::getKeyStatus(SDL_Scancode scancode)
{
    return InputSystem::get()->getKeyStatus(scancode);
}

void Input::getMouseWheel(int &x, int &y)
{
    return InputSystem::get()->getMouseWheel(x, y);
}

ControllerAxisType Input::getControllerAxis(const ControllerAxis &axis)
{
    if (axis == ControllerAxis::AxisLeftY)
        return -(SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis)) + 1);
    return SDL_GameControllerGetAxis(InputSystem::get()->gameController, static_cast<SDL_GameControllerAxis>(axis));
}

ControllerButtonType Input::getControllerButton(const ControllerButton &button)
{
    return SDL_GameControllerGetButton(InputSystem::get()->gameController, static_cast<SDL_GameControllerButton>(button));
}

bool Input::isControllerButtonClicked(const ControllerButton &button)
{
    return InputSystem::get()->isControllerButtonClicked(button);
}

bool Input::isControllerAxisClicked(const ControllerAxis &axis)
{
    // #if SETTINGS_ENGINEMODE && DEBUG
    //     if (Settings::engineMode == Settings::EngineMode::Editor) {
    //         return false;
    //     }
    // #endif
    return InputSystem::get()->isControllerAxisClicked(axis);
}
void Input::getMouse(int &x, int &y, bool onScene)
{
 //   #ifdef DEBUG
 //   auto gameEditor = RenderSystem::get()->getGameEditor();
 //   if (onScene) 
 //   {
 //       gameEditor->mousePressingOnScene(x,y, onScene, 1);
 //       return;
 //   }
 //   #endif
    SDL_GetMouseState(&x, &y);
    y=-y;
}
bool Input::enableRelativeMouse(const bool &enable)
{
    if (enable) return SDL_SetRelativeMouseMode(SDL_TRUE);
    else return SDL_SetRelativeMouseMode(SDL_FALSE);
}

bool Input::isMousePressing(const MouseButton::MouseButton &button)
{
    if (SDL_GetMouseState(NULL, NULL) & button)
    {
        return true;
    }
    else
        false;
}

bool Input::isMouseClicked(const MouseButton::MouseButton &button)
{
    return InputSystem::get()->mouseClickedState[button];
}

} // namespace KuangyeEngine
