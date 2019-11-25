#include "BasicEnemy.hpp"
#include "../ContactFilterDefine.hpp"
#include <random>
#include <wlEngine/Manager/ResourceManager.hpp>
SCRIPT_DEFINATION(Script, BasicEnemy, 32);

extern Entity *player;
BasicEnemy::BasicEnemy(Entity *entity) : Script(entity) {
    entity->AddComponent<StateMachine>("idle");
    Animation *animation = entity->AddComponent<Animation>();
    entity->AddComponent<Audio>();

    entity->AddComponent<LuaScript>("../LuaScript/test.lua");
    createState();
}

void BasicEnemy::createState() {
    auto stateMachine = entity->GetComponent<StateMachine>();
    stateMachine->addTransition("idle", "move", [&]() -> StatePriority { return this->transitionFromIdleToMove(); });
    stateMachine->addTransition("idle", "attack-move", [this]() -> StatePriority { return this->playerDetected(); });
    stateMachine->addTransition("move", "attack-move", [this]() -> StatePriority { return this->playerDetected(); });
    stateMachine->addTransition("move", "idle", [this]() -> StatePriority { return this->moveEnded(); });
    stateMachine->addTransition("attack-move", "idle", [this]() -> StatePriority { return !this->playerDetected(); });
    stateMachine->addTransition("attack-move", "attack", [this]() -> StatePriority { return this->playerIsAttackable(); });
    stateMachine->addTransition("attack", "attack", [this]() -> StatePriority { 
		return this->entity->GetComponent<Animation>()->hasEnded()
		&& this->playerIsAttackable();
		});
    stateMachine->addTransition("attack", "move", [this]() -> StatePriority { 
		return this->entity->GetComponent<Animation>()->hasEnded() && !this->playerIsAttackable(); 
		});
    stateMachine->addTransition("damaged", "move", [this]() -> StatePriority {return this->entity->GetComponent<Animation>()->hasEnded() && !this->playerDetected();});
    stateMachine->addTransition("damaged", "attack-move", [this]() -> StatePriority {return this->entity->GetComponent<Animation>()->hasEnded() && this->playerDetected();});

    /* Idle action group*/
    auto &idleAG = *stateMachine->getActionGroup("idle");
    idleAG[StateActionEnterFuncIndex] = [this]() {
        Animation *animation = this->entity->GetComponent<Animation>();
        animation->playAnimation("idle");
    };

    /* Move action group*/
    auto &moveAG = *stateMachine->getActionGroup("move");
    moveAG[StateActionEnterFuncIndex] = [this]() {
        /* Select a target position */
        wanderRadius = 200;
        float wanderRadiusLower = 80;
        glm::vec2 currentPos = glm::vec2(this->entity->GetComponent<Transform>()->position);

        float radian = ((double)std::rand() / RAND_MAX) * 2 * 3.1415926;

        /* Check this https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c*/
        moveDistance = Utility::genRandomNumber<float>(wanderRadiusLower, wanderRadius); 
        moveVec = glm::vec2(std::cos(radian), std::sin(radian));

        /* Play animation */
        this->entity->GetComponent<Animation>()->playAnimation("move");
    };

    moveAG[StateActionUpdateFuncIndex] = [this]() { this->updateMovement(); };

    /* Attack move action group*/
    auto &attackMoveAG = *stateMachine->getActionGroup("attack-move");
    attackMoveAG[StateActionEnterFuncIndex] = [this]() { this->entity->GetComponent<Animation>()->playAnimation("attack-move"); };

    attackMoveAG[StateActionUpdateFuncIndex] = [this]() {
        glm::vec2 playerPos = player->GetComponent<Transform>()->position;
        auto transform = this->entity->GetComponent<Transform>();
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
		this->entity->GetComponent<Animation>()->playAnimation("attack", false); 
	};

    /* Damaged action group*/
    auto &damagedAG = *stateMachine->getActionGroup("damaged");
    damagedAG[StateActionEnterFuncIndex] = [this]() {
        this->entity->GetComponent<Animation>()->playAnimation("damaged", false);
        this->entity->GetComponent<Audio>()->play("../resource/ballon.wav");
    };
}

void BasicEnemy::updateMovement() {
    Transform *transform = entity->GetComponent<Transform>();
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
    glm::vec2 playerPos = player->GetComponent<Transform>()->position;
    glm::vec2 pos = entity->GetComponent<Transform>()->position;
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
    auto body = entity->GetComponent<TRigidbody>();
    for (auto other : body->contactList) {
        if (other->entity == player) {
            return 6;
        }
    }
    return 0;
}


void BasicEnemy::postInit() {
  auto body = entity->GetComponent<TRigidbody>();
  body->category = CONTACT_FILTER_ENEMY;
  body->mask = CONTACT_FILTER_PLAYER;
  body->setType(BodyType::Dynamic);
  for (auto c : entity->children) {
      c->GetComponent<TRigidbody>()->setType(BodyType::Dynamic);
  }
}

void BasicEnemy::attackedBy(Entity* attackFrom) {
    entity->GetComponent<StateMachine>()->changeState("damaged");
}