#pragma once
#include "Component.hpp"
#include "../GameObject/Entity.hpp"

#include <string>
#include <glm/glm.hpp>
#include <map>
namespace WeilanEngine
{

struct NpcMovement
{
	float from;
	glm::vec2 position;
	std::string animationName;
};

struct NpcConversation
{
	float from;
	float to;
	std::map<std::string, std::string> options;
};

//the system will pick a schedule based on probability
struct NpcOneDayBehavior
{
	float probability;
	std::vector<NpcMovement> movement;
	std::vector<NpcConversation> conversation;
};

class NpcController : public Component
{
	COMPONENT_DECLARATION(Component, NpcController, 32);
	COMPONENT_EDITABLE_DEC();

public:
	/**
		 * @brief
		 *
		 * @param entity
		 * @param id the id that controlls this Ai
		 */
	NpcController(Entity *entity, const std::string &id);
	NpcController(Entity *entity, void **args);

	std::vector<NpcOneDayBehavior> behaviorList;
	std::string id;
	std::string name;
	int todaysBehaviorIndex;
	int currentMovementIndex;
};
} // namespace WeilanEngine
