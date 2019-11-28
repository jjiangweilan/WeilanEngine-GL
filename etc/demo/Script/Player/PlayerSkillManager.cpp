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
        glm::vec2 transform = glm::vec2(player->GetComponent<Transform>()->position);
        auto fireball = EngineManager::GetwlEngine()->getCurrentScene()->CreateGameObject("fireball", nullptr);
        fireball->AddComponent<Transform>(transform.x, transform.y, 0);
        auto sprite = fireball->AddComponent<Sprite>();
        auto animation = fireball->AddComponent<Animation>();
        animation->addAnimationFromAseprite("../resource/player/skill/fireball.json", "../resource/player/skill/fireball.png");
        animation->playAnimation("move");
        float x = Input::getControllerAxis(ControllerAxis::AxisLeftX);
        float y = Input::getControllerAxis(ControllerAxis::AxisLeftY);
        fireball->AddComponent<AiScript::Move>(glm::vec2(x,y), 250);

        PolygonShape shape({50,50});
        auto body = fireball->AddComponent<TRigidbody>(&shape, BodyType::Dynamic);
        body->sensor = true;
        body->mask = CONTACT_FILTER_ENEMY;
        body->contactBegin = [](TRigidbody *body, TRigidbody *other) {
            body->entity->removeComponent<AiScript::Move>();
            auto animation = body->entity->GetComponent<Animation>();
            animation->playAnimation("explode", false);
            body->entity->AddComponent<AiScript::Scheduler>([animation]() { return animation->hasEnded(); }, [animation]() { EngineManager::GetwlEngine()->getCurrentScene()->destroyGameObject(animation->entity); });

            other->entity->GetComponent<BasicEnemy>()->attackedBy(body->entity);
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
        buttonEffect = EngineManager::GetwlEngine()->getCurrentScene()->CreateGameObject("buttonEffect", nullptr);
        glm::vec2 playerTransform = glm::vec2(player->GetComponent<Transform>()->position);
        buttonEffect->AddComponent<Transform>()->setPosition({playerTransform.x, playerTransform.y + 50, 100});
        buttonEffect->AddComponent<Sprite>();
        auto animation = buttonEffect->AddComponent<Animation>();
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
                    buttonEffect->GetComponent<Animation>()->playAnimation("Y");
            };
            if (buttonDown) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'd')
                    spellFailed = true;
                else
                    buttonEffect->GetComponent<Animation>()->playAnimation("B");
            };
            if (buttonRight) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'r')
                    spellFailed = true;
                else
                    buttonEffect->GetComponent<Animation>()->playAnimation("A");
            };
            if (buttonUp) {
                if (currentSpellSkill->spell[currentSpellIndex] != 'u')
                    spellFailed = true;
                else
                    buttonEffect->GetComponent<Animation>()->playAnimation("X");
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
        EngineManager::GetwlEngine()->getCurrentScene()->destroyGameObject(buttonEffect);
        buttonEffect = nullptr;
    }