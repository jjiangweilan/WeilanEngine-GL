#pragma once

#include <wlEngine/wlEngine.hpp>

using namespace wlEngine;
class OpenGLTest : public Script{
    SCRIPT_DECLARATION(Script, OpenGLTest,2);
    public:
    OpenGLTest(Entity* entity);

    void update() override;
    
    private:
        Transform* transform;
        Sprite* sprite;
        TRigidbody* body;

        void draw(const glm::mat4& view, const glm::mat4 proj);
        void drawSprite(const glm::mat4& view, const glm::mat4 proj);
        void drawOutline(const glm::mat4& view, const glm::mat4 proj);
};