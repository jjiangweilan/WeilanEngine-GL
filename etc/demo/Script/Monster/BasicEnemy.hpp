#pragma once
#include <wlEngine/wlEngine.hpp>
#include "../Player/PlayerController.hpp"
using namespace wlEngine;
struct BasicEnemy : public Script {
	SCRIPT_DECLARATION(Script, BasicEnemy, 32);
public:
	Transform* transform;
	TRigidbody* body;
	BasicEnemy(Entity* go);

	float attackCD = 0;
	void update() override {
		updateCoolDown();
	}

	virtual void attackedBy(Entity* attackFrom);

	void postInit() override;

private:
	float timePassedFromLastState = 0;
	glm::vec2 moveVec;
    float wanderRadius;
    float moveDistance = 0;
    glm::vec2 distanceTraveled;
	float moveSpeed = 30.0;
	float attackMoveSpeed = 45.0;

	void createState();

	void attack() {
		if (attackCD == 0) {
			auto scene = EngineManager::GetwlEngine()->getCurrentScene();
			auto goTransform = entity->GetComponent<Transform>();

			auto arrow = scene->CreateGameObject(std::string("arrow"), nullptr);
			arrow->AddComponent<Sprite>("../resource/arrow.png", "Sprite");
			arrow->AddComponent<Transform>(goTransform->position);
			attackCD = 3;
		}
	}

	void updateCoolDown() {
		if (attackCD > 0) attackCD -= Time::deltaTime;
		else attackCD = 0;
	}

	StatePriority transitionFromIdleToMove();
	StatePriority playerDetected();
	StatePriority moveEnded();
	StatePriority playerIsAttackable();

	void idleEnter();
    void updateMovement();
};
