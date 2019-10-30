#include "BuildingScript.hpp"
#include "ContactFilterDefine.hpp"

extern Entity* player;
SCRIPT_DEFINATION(Script, Building, 32);

Building::Building(Entity* entity) : Script(entity){

}

void Building::entranceContactBegin(TRigidbody* other) {
    if (other->entity == player) {
		front->getComponent<Sprite>()->enable = false;
		if (inside) {
			inside->setEnable(true);
		}
    }
	else if (other->entity->getComponent<NpcController>()) {
		other->entity->setParent(this->inside);
		other->entity->setEnable(inside->isEnable());
	}
}

void Building::entranceContactEnd(TRigidbody* other) {
    if (other->entity == player) {
        front->getComponent<Sprite>()->enable = true;
		if (inside) {
			inside->setEnable(false);
		}
    }
	else if (other->entity->getComponent<NpcController>()) {
		other->entity->setParent(nullptr);
		other->entity->setEnable(true);
	}
}

void Building::postInit() {
	for (auto& child : entity->children) {
		if (child->name == "front_building") front = child;
		else if (child->name == "back_building") back = child;
		else if (child->name == "entrance_building") entrance = child;
		else if (child->name == "inside") inside = child;
		else if (child->name == "outline") outline = child;
	}

	auto entranceTrb = entrance->getComponent<TRigidbody>();
	entranceTrb->mask = CONTACT_FILTER_ENEMY | CONTACT_FILTER_PLAYER;
	entranceTrb->sensor = true;
	entranceTrb->contactBegin = [this](TRigidbody* entity, TRigidbody* other) {
		this->entranceContactBegin(other);
	};
	entranceTrb->contactEnd = [this](TRigidbody* entity, TRigidbody* other) {
		this->entranceContactEnd(other);
	};
	inside->setEnable(false);
}


