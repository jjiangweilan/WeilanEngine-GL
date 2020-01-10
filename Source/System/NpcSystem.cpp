#include <random>

#include "System/NpcSystem.hpp"
#include "Manager/EngineManager.hpp"
#include "Component/NpcController.hpp"
#include "Component/Animation.hpp"
#include "Component/TRigidbody.hpp"
#include "GameObject/Entity.hpp"
#include "Global/Time.hpp"
namespace KuangyeEngine
{
NpcSystem *NpcSystem::system = nullptr;

void NpcSystem::init() { system = new NpcSystem(); }

NpcSystem::NpcSystem() {}

void NpcSystem::update()
{
    // pick today's behavior
    auto currentScene = EngineManager::GetKuangyeEngine()->getCurrentScene();
    if (Time::dayTimeInSecond > Time::dayDuration)
    {
        Time::resetDayTime();

        for (auto &npcController : NpcController::collection)
        {
            if (!npcController->entity->IsEnable() || npcController->entity->GetScene() != currentScene)
                continue;
            float remaining = 1;
            float random = 0;
            for (int i = 0; i < npcController->behaviorList.size(); i++)
            {
                random = ((double)rand() / (RAND_MAX)) * remaining;
                float prob = npcController->behaviorList[i].probability;
                if (random < prob)
                {
                    npcController->todaysBehaviorIndex = i; // pick this
                                                            // behavior
                    break;
                }

                remaining -= prob;
            }
            npcController->currentMovementIndex = 0;
        }
    }

    for (auto &npcController : NpcController::collection)
    {
        if (!npcController->entity->IsEnable() || npcController->entity->GetScene() != currentScene)
            continue;
        move(npcController);
    }
}

void NpcSystem::move(NpcController *npcController)
{
    // update npc transform and animation
    auto &behavior =
        npcController->behaviorList[npcController->todaysBehaviorIndex];
    int &i = npcController->currentMovementIndex;
    if (i == behavior.movement.size())
        return;
    auto &movementFrom = behavior.movement[i];
    auto &movementTo = behavior.movement[i + 1];
    if (Time::dayTimeInSecond >= movementFrom.from &&
        Time::dayTimeInSecond < movementTo.from)
    {
        float timePassed = Time::dayTimeInSecond - movementFrom.from;
        float alpha = timePassed / (movementTo.from - movementFrom.from);
        glm::vec2 newPos = movementFrom.position * (1 - alpha) +
                           movementTo.position * alpha;
        auto transform = npcController->entity->GetComponent<Transform>();
        transform->setPosition({newPos.x, newPos.y, transform->position.z});
        auto animation = npcController->entity->GetComponent<Animation>();
        if (!animation->isPlaying(movementFrom.animationName))
            animation->playAnimation(movementFrom.animationName);
    }
    else if (Time::dayTimeInSecond > movementTo.from)
        i++;

    if (i == behavior.movement.size() - 1)
    {
        auto &movementFrom = behavior.movement[i];
        auto animation = npcController->entity->GetComponent<Animation>();
        if (!animation->isPlaying(movementFrom.animationName))
            animation->playAnimation(movementFrom.animationName);
        i++;
    }
}

} // namespace KuangyeEngine
