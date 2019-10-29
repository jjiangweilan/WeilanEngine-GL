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

        //I have this function because SDL treat trigger as axis which is really wired
        static bool isControllerAxisClicked(const ControllerAxis& axis);
    };
}
