#pragma once
#include "System.hpp"
#include <SDL_scancode.h>
#include <SDL_mouse.h>
#include <SDL_joystick.h>
#include <SDL_events.h>
namespace wlEngine {
    class RenderSystem;

	enum class ControllerButton {
		ButtonLeft = SDL_CONTROLLER_BUTTON_X,
		ButtonRight = SDL_CONTROLLER_BUTTON_B,
		ButtonUp = SDL_CONTROLLER_BUTTON_Y,
		ButtonDown = SDL_CONTROLLER_BUTTON_A,
        LeftShoulder = SDL_CONTROLLER_BUTTON_LEFTSHOULDER,
		RightShoulder = SDL_CONTROLLER_BUTTON_RIGHTSHOULDER,
		DpadDown = SDL_CONTROLLER_BUTTON_DPAD_DOWN,
		DpadUp = SDL_CONTROLLER_BUTTON_DPAD_UP,
		DpadLeft = SDL_CONTROLLER_BUTTON_DPAD_LEFT,
		DpadRight = SDL_CONTROLLER_BUTTON_DPAD_RIGHT,
		Start = SDL_CONTROLLER_BUTTON_START,
		Back = SDL_CONTROLLER_BUTTON_BACK,
		RightStick = SDL_CONTROLLER_BUTTON_RIGHTSTICK,
		LeftStick = SDL_CONTROLLER_BUTTON_LEFTSTICK,
		Home = SDL_CONTROLLER_BUTTON_GUIDE
	}; 

	enum class ControllerAxis {
		AxisLeftX = SDL_CONTROLLER_AXIS_LEFTX,
		AxisLeftY = SDL_CONTROLLER_AXIS_LEFTY,
		AxisRightX = SDL_CONTROLLER_AXIS_RIGHTX,
		AxisRightY = SDL_CONTROLLER_AXIS_RIGHTY,
		TriggerLeft = SDL_CONTROLLER_AXIS_TRIGGERLEFT,
		TriggerRight = SDL_CONTROLLER_AXIS_TRIGGERRIGHT,
	};

    //I use this because ControllerButton and Controller Axis is separated bt SDL, hence can't be stored in one array
    enum class ComboInput : uint8_t{
        ButtonUp = 0, ButtonRight = 1, ButtonDown = 2, ButtonLeft = 3, R1 = 4, R2 = 5, L1 = 6, L2 = 7,
    };

    class InputSystem : System {
        SYSTEM_DECLARATION(InputSystem);
    public:
        void getMouseWheel(int& x, int& y);
        Uint8 getKeyStatus(SDL_Scancode&);
    private:
        int mouseX = 0;
        int mouseY = 0;

        bool leftMouseClicked = false;
        bool leftMousePressing = false;
        bool rightMouseClicked = false;
        bool rightMousePresssing = false;

        int wheelX = 0;
        int wheelY = 0;

        /**
         * @brief record if a button has pressed, this is a once event
         *
         */
        std::vector<bool> buttonPressed;
        std::vector<bool> axisPressed;

        void reset();
        void keyDown(const SDL_Event&);
        bool isControllerButtonClicked(const ControllerButton &button);
        bool isControllerAxisClicked(const ControllerAxis& axis);
		SDL_GameController* gameController;
		const int GAMECONTROLLER_AXIS_DEADZONE = 8000;

        friend struct Input;
    };
}
