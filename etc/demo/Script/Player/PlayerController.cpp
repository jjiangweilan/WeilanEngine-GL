#include "PlayerController.hpp"
#include "../Monster/BasicEnemy.hpp"
SCRIPT_DEFINATION(Script, PlayerController, 1);
Entity* player = nullptr;

PlayerController::PlayerController(Entity *entity) : Script(entity), playerSkillManager(entity) {
    animation = entity->AddComponent<Animation>();
    animation->addAnimationFromAseprite("../resource/player/player.json", "../resource/player/player.png");
    animation->playAnimation("idle");
    transform = entity->GetComponent<Transform>();
    createRigidBody();
    createState();
    player = entity;
}

void PlayerController::postInit() {
    camera = EngineManager::GetwlEngine()->getCurrentScene()->getCamera();
}


void PlayerController::createState() {
    playerStates = entity->AddComponent<StateMachine>("idle");

    //transition
    playerStates->addTransition("idle", "attack", []() -> StatePriority { return Input::getKeyStatus(SDL_SCANCODE_SPACE) || Input::isControllerButtonClicked(ControllerButton::ButtonLeft); });
    playerStates->addTransition("attack", "idle", [&]() -> StatePriority { return animation->hasEnded(); });
    playerStates->addTransition("idle", "run", [&]() -> StatePriority {
        float x, y;
        controllerLeftAxisMapping(x, y);
        return x || y;
    });
    playerStates->addTransition("run", "idle", [&]() -> StatePriority {
        float x, y;
        controllerLeftAxisMapping(x, y);
        return x == 0 && y == 0;
    });
    playerStates->addTransition("run", "attack", []() -> StatePriority { return Input::getKeyStatus(SDL_SCANCODE_SPACE) || Input::isControllerButtonClicked(ControllerButton::ButtonLeft); });
    playerStates->addTransition("idle", "collect", []() -> StatePriority {
        return Input::getKeyStatus(SDL_SCANCODE_F);
        // // Input::getControllerButton(ControllerButton::ButtonRight);
    });
    playerStates->addTransition("collect", "idle", [&]() -> StatePriority { return animation->hasEnded(); });
    playerStates->addTransition("run", "combo", [&]() -> StatePriority {return isComboTriggered();});
    playerStates->addTransition("idle", "combo", [&]() -> StatePriority {return isComboTriggered();});
    playerStates->addTransition("combo", "run", [&]() -> StatePriority { if (!Input::getControllerAxis(ControllerAxis::TriggerRight)) return 1; else return 0;});
    playerStates->addTransition("combo", "idle", [&]() -> StatePriority { if (!Input::getControllerAxis(ControllerAxis::TriggerRight) || playerSkillManager.spellFailed || playerSkillManager.spellSucceed) return 2; else return 0;});
    playerStates->addTransition("combo", "attack", [&]() -> StatePriority { if (!Input::getControllerAxis(ControllerAxis::TriggerRight)) return 3; else return 0;});

    //transition behavior
    StateActionGroup &standActions = *playerStates->getActionGroup("idle");
    standActions[StateActionEnterFuncIndex] = [&]() {
        // rigidbody->setVelocity(0, 0, 0);
        animation->playAnimation("idle");
    };

    //walk
    StateActionGroup &walkActions = *playerStates->getActionGroup("run");
    walkActions[StateActionEnterFuncIndex] = [&]() { updateSpeed(); };
    walkActions[StateActionUpdateFuncIndex] = [&]() {
        updateSpeed();

        if (lookAtVec.x < -0.8 && !animation->isPlaying("run-front-left"))
            animation->playAnimation("run-front-left");
        else if (lookAtVec.x >= -0.8 && lookAtVec.x <= 0.8 && !animation->isPlaying("run-front"))
            animation->playAnimation("run-front");
        else if (lookAtVec.x > 0.8 && !animation->isPlaying("run-front-right"))
            animation->playAnimation("run-front-right");
    };

    //attack
    StateActionGroup &attackActions = *playerStates->getActionGroup("attack");
    attackActions[StateActionEnterFuncIndex] = [&]() {
        // rigidbody->setVelocity(0, 0, 0);
        animation->playAnimation("attack", false);
    };
    attackActions[StateActionUpdateFuncIndex] = [&]() {
        if (animation->getCurrentFrame() == 2 && attackQuery == nullptr) {
            PolygonShape shape(80, 80, 0, {-50,0});
            TRigidbody queryBody(entity, &shape, BodyType::Dynamic);
            attackQuery = &queryBody;
            queryBody.sensor = true;
            Physics::query(queryBody);

            for (auto& contact : queryBody.contactList) {
                if (auto basicEnemy = contact->entity->GetComponent<BasicEnemy>()) {
                    basicEnemy->attackedBy(entity);
                }
            }
        }
    };

    attackActions[StateActionExitFuncIndex] = [&]() { attackQuery = nullptr; };

    //pick
    StateActionGroup &pickActions = *playerStates->getActionGroup("collect");
    pickActions[StateActionEnterFuncIndex] = [&]() {

    };
    pickActions[StateActionExitFuncIndex] = [&]() { };

    //combo
    StateActionGroup &comboActions = *playerStates->getActionGroup("combo");
    comboActions[StateActionEnterFuncIndex] = [&]() {
        auto aniamtion = entity->GetComponent<Animation>();
        animation->playAnimation("combo");
        playerSkillManager.startSpellSkill();
    };
    comboActions[StateActionUpdateFuncIndex] = [&]() {
        playerSkillManager.updateSpellSkill();
    };
    comboActions[StateActionExitFuncIndex] = [&]() {
        playerSkillManager.endSpellSkill();
    };
}

void PlayerController::createRigidBody() {
    auto shape = PolygonShape(18, 20, 0, {0, -30});
    shape.leftPoint = {-12, -40};
    shape.rightPoint = {12, -40};
    // auto shape = CircleShape({0,-30}, 10);
    rigidbody = entity->AddComponent<TRigidbody>(&shape, BodyType::Dynamic);
    rigidbody->category = CONTACT_FILTER_PLAYER;

    rigidbody->contactBegin = [](TRigidbody* entity, TRigidbody *other) {
        if (other->category == CONTACT_FILTER_ENVIORNMENT_OBJECT_OUTLINE) {
            auto parent = other->entity->GetParent();
            for (auto c : parent->children) {
                if (c->name == "front_building") {
                    c->GetComponent<Sprite>()->transparency = 0.5;
                    break;
                }
            }
        }
    };

    rigidbody->contactEnd = [](TRigidbody* entity, TRigidbody *other) {
        if (other->category == CONTACT_FILTER_ENVIORNMENT_OBJECT_OUTLINE) {
            auto parent = other->entity->GetParent();
            for (auto c : parent->children) {
                if (c->name == "front_building") {
                    c->GetComponent<Sprite>()->transparency = 1.0;
                    break;
                }
            }
        }
    };

    rigidbody->shouldCollide = std::bind(&PlayerController::shouldCollide, this, std::placeholders::_1);
}

void PlayerController::controllerLeftAxisMapping(float &x, float &y) {
    const int DEAD_ZONE = 10000;
    ControllerAxisType xx, yy;
    xx = Input::getControllerAxis(ControllerAxis::AxisLeftX);
    yy = Input::getControllerAxis(ControllerAxis::AxisLeftY); 
    int radius2 = xx * xx + yy * yy;
    if (radius2 < 100000000) {
        xx = 0;
        yy = 0;
    }
    if (xx > 20000)
        xx = 20000;
    else if (xx < -20000)
        xx = -20000;
    if (yy > 20000)
        yy = 20000;
    else if (yy < -20000)
        yy = -20000;
    x = (float)xx / Input::AXIS_MAX;
    y = (float)yy / Input::AXIS_MAX;
}

void PlayerController::updateSpeed() {
    glm::vec2 moveVec;
    controllerLeftAxisMapping(moveVec.x, moveVec.y);
    moveVec.x *= 300 * Time::deltaTime;
    moveVec.y *= 300 * Time::deltaTime;

    lookAtVec = glm::normalize(moveVec);

    transform->moveBy(moveVec.x, moveVec.y, 0);
    updateCamera(moveVec);
}

void PlayerController::updateCamera(const glm::vec2& playerMovesBy) {
    glm::vec2 playerPos = glm::vec2(entity->GetComponent<Transform>()->position);

    auto transformC = camera->GetComponent<Transform>();
    glm::vec2 cameraPos = glm::vec2(transformC->position);
    if (std::abs(cameraPos.x + 640 - playerPos.x ) > 200) {
        transformC->moveBy(playerMovesBy.x, 0);
    }
    if (std::abs(cameraPos.y + 360 - playerPos.y ) > 200) {
        transformC->moveBy(0, playerMovesBy.y);
    }
}

StatePriority PlayerController::isComboTriggered() {
    return Input::isControllerAxisClicked(ControllerAxis::TriggerRight);
}