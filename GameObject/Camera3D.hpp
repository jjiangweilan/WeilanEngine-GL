#ifndef CAMERA_H
#define CAMERA_H

#include "Camera.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

namespace wlEngine {
    class Camera3D : public Camera {
    public:
        Camera3D();
        ~Camera3D();

		glm::mat4 getViewMatrix() override;
        void update() override;

        glm::vec3 front;
        glm::vec3 right;
        const glm::vec3 WORLD_UP = glm::vec3(0,1,0);

        //mouse
        int relX;
        int relY;
        float sensitivity = 0.3;
		bool enableMouse = false;
        float pitch = 0;
        float yaw = -90;

        float speed = 5;
    private:
        void updateEyeDirection();
        void updatePosition();
    };
}

#endif
