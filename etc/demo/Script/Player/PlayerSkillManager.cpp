#include "../ContactFilterDefine.hpp"
#include "PlayerSkillManager.hpp"
#include "../AiScript/Move.hpp"
#include "../AiScript/Scheduler.hpp"
#include "../Monster/BasicEnemy.hpp"
namespace PlayerSkill {
    FireBall::FireBall() {
        spell = "lldl"; //left left down left
        useDifficulty = 3;
    }
    void FireBall::useSkill() {
        
    }

    Entity* FireBall::castSpell(Entity* player) {
        glm::vec2 transform = glm::vec2(player->getComponent<Transform>()->position);
        auto fireball = EngineManager::getwlEngine()->getCurrentScene()->createGameObject("fireball", nullptr);
        fireball->addComponent<Transform>(transform.x, transform.y, 0);
        auto sprite = fireball->addComponent<Sprite>();
        auto animation = fireball->addComponent<Animation>();
        animation->addAnimationFromAseprite("../resource/player/skill/fireball.json", "../resource/player/skill/fireball.png");
        animation->playAnimation("move");
        float x = Input::getControllerAxis(ControllerAxis::AxisLeftX);
        float y = Input::getControllerAxis(ControllerAxis::AxisLeftY);
        fireball->addComponent<AiScript::Move>(glm::vec2(x,y), 250);

        PolygonShape shape({50,50});
        auto body = fireball->addComponent<TRigidbody>(&shape, BodyType::Dynamic);
        body->sensor = true;
        body->mask = CONTACT_FILTER_ENEMY;
        body->contactBegin = [](TRigidbody *body, TRigidbody *other) {
            body->entity->removeComponent<AiScript::Move>();
            auto animation = body->entity->getComponent<Animation>();
            animation->playAnimation("explode", false);
            body->entity->addComponent<AiScript::Scheduler>([animation]() { return animation->hasEnded(); }, [animation]() { EngineManager::getwlEngine()->getCurrentScene()->destroyGameObject(animation->entity); });

            other->entity->getComponent<BasicEnemy>()->attackedBy(body->entity);
        };
        return fireball;
    }
    } // namespace PlayerSkill

    PlayerSkillManager::PlayerSkillManager(Entity *player) : player(player) {
        currentSpellSkill = &fireBall;
        currentSpellIndex = 0;
    };
    void PlayerSkillManager::startSpellSkill() {
        spellFailed = false;
        spellSucceed = false;
        currentSpellIndex = 0;
        buttonEffect = EngineManager::getwlEngine()->getCurrentScene()->createGameObject("buttonEffect", nullptr);
        glm::vec2 playerTransform = glm::vec2(player->getComponent<Transform>()->position);
        buttonEffect->addComponent<Transform>()->setPosition({playerTransform.x, playerTransform.y + 50, 100});
        buttonEffect->addComponent<Sprite>();
        auto animation = buttonEffect->addComponent<Animation>();
        animation->addAnimationFromAseprite("../resource/player/button.json", "../resource/player/button.png");
        animation->playAnimation("empty");
    }
    void PlayerSkillManager::updateSpellSkill() {
        bool buttonLeft = Input::isControllerButtonClicked(ControllerButton::ButtonLeft);
        bool buttonDown = Input::isControllerButtonClicked(ControllerButton::ButtonDown);
        bool buttonUp = Input::isControllerButtonClicked(ControllerButton::ButtonUp);
        bool buttonRight = Input::isControllerButtonClicked(ControllerButton::ButtonRight);

        if (currentSpellIndex < currentSpellSkill->spell.size() && (buttonLeft || buttonDown || buttonUp || buttonRight)) {
            if (buttonLeft) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'l')
                    spellFailed = true;
                else
                    buttonEffect->getComponent<Animation>()->playAnimation("Y");
            };
            if (buttonDown) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'd')
                    spellFailed = true;
                else
                    buttonEffect->getComponent<Animation>()->playAnimation("B");
            };
            if (buttonRight) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'r')
                    spellFailed = true;
                else
                    buttonEffect->getComponent<Animation>()->playAnimation("A");
            };
            if (buttonUp) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'u')
                    spellFailed = true;
                else
                    buttonEffect->getComponent<Animation>()->playAnimation("X");
            };
            currentSpellIndex++;
            if (currentSpellIndex == currentSpellSkill->spell.size() && !spellFailed)
                spellSucceed = true;
        }
    }
    void PlayerSkillManager::endSpellSkill() {
        if (spellSucceed) {
            currentSpellSkill->castSpell(player);
        }
        EngineManager::getwlEngine()->getCurrentScene()->destroyGameObject(buttonEffect);
        buttonEffect = nullptr;
    }