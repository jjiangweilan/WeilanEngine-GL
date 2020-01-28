#pragma once
#include "Component.hpp"
#include <map>
#include <string>
#include <functional>
#include <array>
namespace WeilanEngine
{
using StateType = std::string;
using StatePriority = uint8_t;
using StateConditionType = std::function<StatePriority()>;
using StateActionType = std::function<void()>;
using StateMachineType = std::map<StateType, std::map<StateType, StateConditionType>>;
using StateActionGroup = std::array<StateActionType, 3>;

class StateMachine : public Component
{
	COMPONENT_DECLARATION(Component, StateMachine, 8);

public:
	StateMachine(Entity *, const StateType &initialState);
	/**
			 * @brief set the states from state to state with their condition
			 *
			 * @param from
			 * @param to
			 * @param condition, a function returns a unsigned int that indicates the priority of the condition
			 */
	void addTransition(const StateType &from, const StateType &to, StateConditionType condition);
	void addState(const StateType &name);
	StateActionGroup *getActionGroup(const std::string &stateName);
	void changeState(const StateType &newState);
	const StateType &getCurrentState();

private:
	StateType currentState;
	StateMachineType states;

#define StateActionEnterFuncIndex 0
#define StateActionUpdateFuncIndex 1
#define StateActionExitFuncIndex 2
	std::map<StateType, StateActionGroup> stateActions;

	friend class StateMachineSystem;
};
} // namespace WeilanEngine
