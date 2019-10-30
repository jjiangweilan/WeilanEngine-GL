#include "BasicEnemy.hpp"
#include "../ContactFilterDefine.hpp"
#include <random>
#include <wlEngine/Manager/ResourceManager.hpp>
SCRIPT_DEFINATION(Script, BasicEnemy, 32);

extern Entity *player;
BasicEnemy::BasicEnemy(Entity *entity) : Script(entity) {
    entity->addComponent<StateMachine>("idle");
    Animation *animation = entity->addComponent<Animation>();
    entity->addComponent<Audio>();

    entity->addComponent<LuaScript>("../LuaScript/test.lua");
    createState();
}

void BasicEnemy::createState() {
    auto stateMachine = entity->getComponent<StateMachine>();
    stateMachine->addTransition("idle", "move", [&]() -> StatePriority { return this->transitionFromIdleToMove(); });
    stateMachine->addTransition("idle", "attack-move", [this]() -> StatePriority { return this->playerDetected(); });
    stateMachine->addTransition("move", "attack-move", [this]() -> StatePriority { return this->playerDetected(); });
    stateMachine->addTransition("move", "idle", [this]() -> StatePriority { return this->moveEnded(); });
    stateMachine->addTransition("attack-move", "idle", [this]() -> StatePriority { return !this->playerDetected(); });
    stateMachine->addTransition("attack-move", "attack", [this]() -> StatePriority { return this->playerIsAttackable(); });
    stateMachine->addTransition("attack", "attack", [this]() -> StatePriority { 
		return this->entity->getComponent<Animation>()->hasEnded()
		&& this->playerIsAttackable();
		});
    stateMachine->addTransition("attack", "move", [this]() -> StatePriority { 
		return this->entity->getComponent<Animation>()->hasEnded() && !this->playerIsAttackable(); 
		});
    stateMachine->addTransition("damaged", "move", [this]() -> StatePriority {return this->entity->getComponent<Animation>()->hasEnded() && !this->playerDetected();});
    stateMachine->addTransition("damaged", "attack-move", [this]() -> StatePriority {return this->entity->getComponent<Animation>()->hasEnded() && this->playerDetected();});

    /* Idle action group*/
    auto &idleAG = *stateMachine->getActionGroup("idle");
    idleAG[StateActionEnterFuncIndex] = [this]() {
        Animation *animation = this->entity->getComponent<Animation>();
        animation->playAnimation("idle");
    };

    /* Move action group*/
    auto &moveAG = *stateMachine->getActionGroup("move");
    moveAG[StateActionEnterFuncIndex] = [this]() {
        /* Select a target position */
        wanderRadius = 200;
        float wanderRadiusLower = 80;
        glm::vec2 currentPos = glm::vec2(this->entity->getComponent<Transform>()->position);

        float radian = ((double)std::rand() / RAND_MAX) * 2 * 3.1415926;

        /* Check this https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c*/
        moveDistance = Utility::genRandomNumber<float>(wanderRadiusLower, wanderRadius); 
        moveVec = glm::vec2(std::cos(radian), std::sin(radian));

        /* Play animation */
        this->entity->getComponent<Animation>()->playAnimation("move");
    };

    moveAG[StateActionUpdateFuncIndex] = [this]() { this->updateMovement(); };

    /* Attack move action group*/
    auto &attackMoveAG = *stateMachine->getActionGroup("attack-move");
    attackMoveAG[StateActionEnterFuncIndex] = [this]() { this->entity->getComponent<Animation>()->playAnimation("attack-move"); };

    attackMoveAG[StateActionUpdateFuncIndex] = [this]() {
        glm::vec2 playerPos = player->getComponent<Transform>()->position;
        auto transform = this->entity->getComponent<Transform>();
        glm::vec2 pos = transform->position;

        glm::vec2 moveVec = glm::normalize(playerPos - pos);
        float moveDelta = Time::deltaTime * this->attackMoveSpeed;
        float moveX = moveVec.x * moveDelta;
        float moveY = moveVec.y * moveDelta;
        transform->moveBy(moveX, moveY);
    };

    /* Attack action group*/
    auto &attackAG = *stateMachine->getActionGroup("attack");
    attackAG[StateActionEnterFuncIndex] = [this]() { 
		this->entity->getComponent<Animation>()->playAnimation("attack", false); 
	};

    /* Damaged action group*/
    auto &damagedAG = *stateMachine->getActionGroup("damaged");
    damagedAG[StateActionEnterFuncIndex] = [this]() {
        this->entity->getComponent<Animation>()->playAnimation("damaged", false);
        this->entity->getComponent<Audio>()->play("../resource/ballon.wav");
    };
}

void BasicEnemy::updateMovement() {
    Transform *transform = entity->getComponent<Transform>();
    float moveDelta = Time::deltaTime * moveSpeed;
    float moveX = moveVec.x * moveDelta;
    float moveY = moveVec.y * moveDelta;
    transform->moveBy(moveX, moveY);

    distanceTraveled += glm::vec2(moveX, moveY);
}

StatePriority BasicEnemy::transitionFromIdleToMove() {
    // AI start to move after it idles for 3 seconds
    if (timePassedFromLastState > 3) {
        timePassedFromLastState = 0;
        return 1;
    }

    timePassedFromLastState += Time::deltaTime;
    return 0;
}

StatePriority BasicEnemy::playerDetected() {
    glm::vec2 playerPos = player->getComponent<Transform>()->position;
    glm::vec2 pos = entity->getComponent<Transform>()->position;
    float distance = glm::length(pos - playerPos);

    if (distance < 240) {
        return 5;
    }

    return 0;
}

StatePriority BasicEnemy::moveEnded() {
    if (glm::length(distanceTraveled) > moveDistance) {
        distanceTraveled.x = 0;
        distanceTraveled.y = 0;
        return 1;
    }
    return 0;
}
StatePriority BasicEnemy::playerIsAttackable() {
    auto body = entity->getComponent<TRigidbody>();
    for (auto other : body->contactList) {
        if (other->entity == player) {
            return 6;
        }
    }
    return 0;
}


void BasicEnemy::postInit() {
  auto body = entity->getComponent<TRigidbody>();
  body->category = CONTACT_FILTER_ENEMY;
  body->mask = CONTACT_FILTER_PLAYER;
  body->setType(BodyType::Dynamic);
  for (auto c : entity->children) {
      c->getComponent<TRigidbody>()->setType(BodyType::Dynamic);
  }
}

void BasicEnemy::attackedBy(Entity* attackFrom) {
    entity->getComponent<StateMachine>()->changeState("damaged");
}