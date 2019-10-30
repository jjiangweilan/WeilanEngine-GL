#include "StateMachine.hpp"
namespace wlEngine
{
COMPONENT_DEFINATION(Component, StateMachine, 8);
StateMachine::StateMachine(Entity *go, const StateType &initialState) : Component(go), currentState(initialState)
{
}

void StateMachine::addTransition(const StateType &from, const StateType &to, StateConditionType condition)
{
    states[from][to] = condition;
}

StateActionGroup *StateMachine::getActionGroup(const std::string &stateName)
{
    return &stateActions[stateName];
}

const StateType &StateMachine::getCurrentState()
{
    return currentState;
}

void StateMachine::changeState(const StateType &newState)
{
    const auto &exitFunc = stateActions[currentState][StateActionExitFuncIndex];
    if (exitFunc)
        exitFunc();

    currentState = newState;
    const auto &enterFunc = stateActions[currentState][StateActionEnterFuncIndex];
    if (enterFunc)
        enterFunc();
}
void StateMachine::addState(const StateType &name)
{
    states[name];
}
} // namespace wlEngine
