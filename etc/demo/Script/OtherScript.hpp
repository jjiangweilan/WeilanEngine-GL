#pragma once
#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;
struct AnnoShader : public Script {
	SCRIPT_DECLARATION(Script, AnnoShader, 1);
	Transform* light1Transform = nullptr;
	Transform* light2Transform = nullptr;

	const Graphics::Shader* shader;
	AnnoShader(Entity* go) : Script(go) {
		shader = go->getComponent<Model>()->getShader();
		shader->use();
		shader->setVec3("light1.ambient", 0.3f, 0.3f, 0.3f);
		shader->setVec3("light1.diffuse", 0.8f, 0.8f, 0.8f);
		shader->setVec3("light1.specular", 1.0f, 1.0f, 1.0f);

		shader->setVec3("light2.ambient", 0.3f, 0.3f, 0.3f);
		shader->setVec3("light2.diffuse", 0.8f, 0.8f, 0.8f);
		shader->setVec3("light2.specular", 1.0f, 1.0f, 1.0f);
	}

	void update() override {
		shader->use();
		shader->setVec3("light1.position", light1Transform->position);
		shader->setVec3("light2.position", light2Transform->position);

	}
};
SCRIPT_DEFINATION(Script, AnnoShader, 1);

//struct Rotate : public Script {
//	SCRIPT_DECLARATION(Script, Rotate, 1);
//
//	Transform* transform;
//	Rotate(Entity* go) : Script(go) {
//		transform = go->getComponent<Transform>();
//		transform->setScale(0.05, 0.05, 0.05);
//		transform->setPosition({ 0,0,0 });
//	}
//
//	void update() override {
//		transform->setPosition({ 1.0 * glm::sin(Time::timeAfterGameStart), 1 + 1.0 * glm::sin(Time::timeAfterGameStart), 1.0 * glm::cos(Time::timeAfterGameStart) });
//	}
//};
//
//SCRIPT_DEFINATION(Script, Rotate, 1);


struct SpriteRotate : public Script {
	SCRIPT_DECLARATION(Script, SpriteRotate, 1);

	Transform* transform;
	SpriteRotate(Entity* go) : Script(go) {
		transform = go->getComponent<Transform>();
	}

	void update() override {
		transform->rotate({ 0,1,0 }, 1);
	}
};
SCRIPT_DEFINATION(Script, SpriteRotate, 1);

struct Rotate1 : public Script {
	SCRIPT_DECLARATION(Script, Rotate1, 1);

	Transform* transform;
	const Graphics::Shader* shader;
	Rotate1(Entity* go) : Script(go) {
		shader = go->getComponent<Model>()->getShader();
		transform = go->getComponent<Transform>();
		transform->setScale(0.05, 0.05, 0.05);
		transform->setPosition({ 0,0,0 });
		shader->setVec3("objectColor", { 1,1,1 });
		shader->setVec3("lightColor", { 1,1,1 });
	}

	void update() override {
		transform->setPosition({ 1.0 * glm::cos(Time::timeAfterGameStart), 1 - 1.0 * glm::cos(Time::timeAfterGameStart), 1.0 * glm::sin(Time::timeAfterGameStart) });
	}
};
SCRIPT_DEFINATION(Script, Rotate1, 1);
