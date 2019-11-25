#pragma once

#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;

class EnviornmentObject : public Script {
	SCRIPT_DECLARATION(Script, EnviornmentObject, 32);

public:
	TRigidbody* body;
	EnviornmentObject(Entity* entity) : Script(entity) {
		LineShape shape({ {-90,-66}, {-50,-34}, {90,-105}, { 53,-139 } });
		//shape.pushbackVertex({-300,-300});
		//CircleShape shape({0,0},60);
		body = entity->AddComponent<TRigidbody>(&shape, BodyType::Static);
	};

	void update() override {

	}
};
