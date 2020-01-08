#include "Component/NpcController.hpp"
#include "Manager/ResourceManager.hpp"

#include <fstream>

namespace wlEngine {

COMPONENT_DEFINATION(Component, NpcController, 32);
COMPONENT_EDITABLE_DEF(NpcController);

NpcController::NpcController(Entity *entity, const std::string &id)
    : Component(entity), todaysBehaviorIndex(0), currentMovementIndex(0)  {
    auto resourceManager = ResourceManager::get();
    Json &npcData = resourceManager->getNpcJsonData()[id];
    this->id = id;
    name = npcData["name"].get<std::string>();

    // parse schedule
    for (Json::iterator it = npcData["one-day-behavior-list"].begin();
         it != npcData["one-day-behavior-list"].end(); ++it) {
        Json &data = *it;
        NpcOneDayBehavior behavior;
        behavior.probability = data["probability"].get<float>();

        for (auto &move : data["behavior-list"]["movement"]) {
            NpcMovement movement;
            movement.from = move["from"].get<int>();
            movement.position = glm::vec2(move["position"][0].get<int>(),
                                          move["position"][1].get<int>());
            movement.animationName = move["animation"].get<std::string>();
            behavior.movement.push_back(movement);
        }

        for (Json::iterator it = data["behavior-list"]["conversation"].begin();
             it != data["behavior-list"]["conversation"].end(); ++it) {
            auto &data = *it;
            NpcConversation conversation;
            conversation.from = data["from"].get<int>();
            conversation.to = data["to"].get<int>();

            for (auto &conver : data["options"]) {
                conversation.options[conver["question"].get<std::string>()] =
                    conver["answer"].get<std::string>();
            }

            behavior.conversation.push_back(conversation);
        }

        this->behaviorList.push_back(behavior);
    }
}

NpcController::NpcController(Entity *entity, void **args) : Component(entity) {

}

} // namespace wlEngine
