#include "Camera3D.hpp"
#include "../Time.hpp"
#include "../Component/Transform.hpp"
namespace wlEngine {
    Camera3D::Camera3D() : Camera(), front(0,0, -1),right(1,0,0) {
		SDL_GetMouseState(&relX, &relY);
        
    };
    Camera3D::~Camera3D() {};

	glm::mat4 Camera3D::getViewMatrix() {
		auto transform = getComponent<Transform>();
		return glm::lookAt(transform->position, transform->position + front, glm::cross(right, front));
	}
	
    void Camera3D::update() {
        updatePosition();
        updateEyeDirection();
        if (SDL_GetMouseState(nullptr, nullptr) & SDL_BUTTON(SDL_BUTTON_LEFT)) {
            SDL_SetRelativeMouseMode(SDL_TRUE);
            enableMouse = true;
        }
        else {
            SDL_SetRelativeMouseMode(SDL_FALSE);
            enableMouse = false;
        }

    }
    void Camera3D::updateEyeDirection() {
        if (enableMouse) {
            SDL_GetRelativeMouseState(&relX, &relY);
            yaw += relX * sensitivity;
            pitch -= relY * sensitivity;
        }

        if (pitch > 89.0f)
            pitch = 89.0f;
        if (pitch < -89.0f)
            pitch = -89.0f;

        float yawRadians = glm::radians(yaw);
        float pitchRadians = glm::radians(pitch);
        front.x = glm::cos(yawRadians) * glm::cos(pitchRadians);
        front.y = glm::sin(pitchRadians);
        front.z = glm::sin(yawRadians) * glm::cos(pitchRadians);
        front = glm::normalize(front);
    }
    void Camera3D::updatePosition() {
        auto keyboard = SDL_GetKeyboardState(nullptr);
        right = glm::normalize(glm::cross(front, WORLD_UP));

        float speedDelta = speed * Time::deltaTime;
        auto transform = getComponent<Transform>();
        if (keyboard[SDL_SCANCODE_LEFT]) {
            transform->position -= right * speedDelta; 
        }
        if (keyboard[SDL_SCANCODE_RIGHT]) {
            transform->position += right * speedDelta;
        }
        if (keyboard[SDL_SCANCODE_UP]) {
            transform->position += front * speedDelta;
        }
        if (keyboard[SDL_SCANCODE_DOWN]) {
            transform->position -= front * speedDelta;
        }
    }
}

