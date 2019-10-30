#include "StateMachineSystem.hpp"
#include "../EngineManager.hpp"
#include "../Component/StateMachine.hpp"
#include "../GameObject/Entity.hpp"
namespace wlEngine
{
SYSTEM_DEFINATION(StateMachineSystem);

StateMachineSystem::StateMachineSystem()
{
}

void StateMachineSystem::update()
{
    auto currentScene = EngineManager::getwlEngine()->getCurrentScene();
    for (auto &sm : StateMachine::collection)
    {
        if (!sm->entity->isEnable() || sm->entity->getScene() != currentScene)
        {
            continue;
        }
        StatePriority max_ = 0;
        const StateType &currStateTemp = sm->currentState;
        const StateType *newState = nullptr;
        for (auto &stateMap : sm->states[currStateTemp])
        {
            if (StatePriority priority = stateMap.second() > max_)
            {
                max_ = priority;
                newState = &stateMap.first;
            }
        }

        if (newState != nullptr)
        {
            const auto &exitFunc = sm->stateActions[currStateTemp][StateActionExitFuncIndex];
            if (exitFunc)
                exitFunc();
            sm->currentState = *newState;
            const auto &enterFunc = sm->stateActions[sm->currentState][StateActionEnterFuncIndex];
            if (enterFunc)
                enterFunc();
        }
        else
        {
            const auto &updateFunc = sm->stateActions[currStateTemp][StateActionUpdateFuncIndex];
            if (updateFunc)
                updateFunc();
        }
    }
}
} // namespace wlEngine
