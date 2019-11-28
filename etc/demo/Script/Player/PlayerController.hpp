#pragma once
#include "../ContactFilterDefine.hpp"
#include <wlEngine/wlEngine.hpp>
#include "PlayerSkillManager.hpp"
using namespace wlEngine;

extern Entity *player;

struct PlayerController : public Script {
    SCRIPT_DECLARATION(Script, PlayerController, 1);
  public:
    Transform *transform = nullptr;
    Animation *animation = nullptr;
    TRigidbody *rigidbody = nullptr;
    StateMachine *playerStates = nullptr;
    TRigidbody *attackQuery = nullptr;
    Entity* camera = nullptr;
    Sprite *sprite = nullptr;
    glm::vec2 lookAtVec;

    PlayerController(Entity *entity);
    PlayerSkillManager playerSkillManager;

    void update() override {

        // I am testing dialog function here
        static Entity *dialog = nullptr;
        auto clicked = Input::isControllerButtonClicked(ControllerButton::ButtonRight);
        if (clicked && !dialog) { // player wants to interact with something
            PolygonShape shape(60, 60, 0, {0, -30});
            TRigidbody contactCheck(this->entity, &shape, BodyType::Dynamic);
            contactCheck.mask = CONTACT_FILTER_INTERACTABLE;
            Physics::query(contactCheck);
            if (contactCheck.contactList.size() > 0) {
                Entity *npc = contactCheck.contactList[0]->entity;
                NpcController *npcController = npc->GetComponent<NpcController>();
                dialog = UserInterface::showDialog(npcController);
            }
        } else if (clicked && dialog) {
            EngineManager::GetwlEngine()->getCurrentScene()->destroyGameObject(dialog);
            dialog = nullptr;
        }
    }

    void postInit() override;

  private:
    void createState();
    void updateCamera(const glm::vec2& playerMovesBy);

    void createRigidBody();

    void controllerLeftAxisMapping(float &x, float &y);

    void updateSpeed();

    bool shouldCollide(TRigidbody *other) { return true; }
    StatePriority isComboTriggered();
};
