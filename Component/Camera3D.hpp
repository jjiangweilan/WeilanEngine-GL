#pragma once
#include "Camera.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <SDL.h>

namespace wlEngine {
    class Camera3D : public Camera {
    public:
    COMPONENT_DECLARATION_NEW(Camera, Camera3D);
    COMPONENT_EDITABLE_DEC();
	public:
        Camera3D(Entity* entity);
        Camera3D(Entity* entity, void** data);
        ~Camera3D();

        virtual glm::mat4 GetViewMatrix() const override;
        virtual glm::mat4 GetProjMatrix() const override;
        void update() override;

        glm::vec3 front;
        glm::vec3 right;
        const glm::vec3 WORLD_UP = glm::vec3(0,1,0);

        //mouse
        int relX;
        int relY;
        float sensitivity;
		bool enableMouse = false;
        float pitch = 0;
        float yaw = -90;

        float speed;
        void SetProjectionMatrix(const float &fovy, const float &aspect,
                                 const float &zNear, const float &zFar) override;

    private:
        glm::mat4 proj;
        void updateEyeDirection();
        void updatePosition();
    };
}
