#pragma once
#include <vector>

#include "System/InputSystem.hpp"
namespace wlEngine {
    using ControllerAxisType=Sint16;
	using ControllerButtonType=Uint8;

    /**
     * @brief Input class is used to hide InputSystem from Gameplay Code
     */
    struct Input {
		static std::vector<bool> buttonPressed;
        static const ControllerAxisType AXIS_MAX = 0x7fff;

		/* Keyboard ********************************************/
        static int getKeyStatus(SDL_Scancode); 
        static void getMouseWheel(int& x, int& y);

		/* Controller ********************************************/
        static ControllerAxisType getControllerAxis(const ControllerAxis& axis);
		static ControllerButtonType getControllerButton(const ControllerButton& button);
		static bool isControllerButtonClicked(const ControllerButton& button);

        /* Mouse ************************************************/
        static void getMouse(int& x, int& y, bool onScene=true);
        static bool isMousePressing(const MouseButton::MouseButton& buton);
        static bool isMouseClicked(const MouseButton::MouseButton& button);
        /**
         * @brief this function hides cursor
         * Note: this solves the problem that the relative mouse calculated
         * by SDL_GetMouseState between frames is 0
         * 
         * @param enable 
         * @return true 
         * @return false 
         */
        static bool enableRelativeMouse(const bool& enable);

        //I have this function because SDL treat trigger as axis which is really wired
        static bool isControllerAxisClicked(const ControllerAxis& axis);
    };
}
