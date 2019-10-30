#include "Move.hpp"

namespace AiScript {
SCRIPT_DEFINATION(Script, Move, 8);
Move::Move(Entity* e) : Script(e){};
Move::Move(Entity* e, const glm::vec2& dir, const float& speed) : Script(e), speed(speed) {
    direction = glm::normalize(dir);
    existTime = 0;
};

void Move::update() {
    existTime += Time::deltaTime;
    auto transform = entity->getComponent<Transform>();
    transform->moveBy(direction.x * Time::deltaTime * speed, direction.y * Time::deltaTime * speed);
    if (existTime > 2) 
    EngineManager::getwlEngine()->getCurrentScene()->destroyGameObject(entity);
}
};