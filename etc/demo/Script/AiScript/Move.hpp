#pragma once
#include <wlEngine/wlEngine.hpp>

using namespace wlEngine;

namespace AiScript {
class Move : public Script {
    SCRIPT_DECLARATION(Script, Move, 8);
    glm::vec2 direction;
    float speed;
    float existTime;
    Move(Entity *, const glm::vec2 &direction, const float& speed);
    void update() override;

  private:
    Move(Entity *);
};
}; // namespace AiScript