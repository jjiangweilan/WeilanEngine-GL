#include "Script/ContactFilterDefine.hpp"
#include "Script/OtherScript.hpp"
#include "Script/Player/PlayerController.hpp"
#include "Script/OpenGLTest.hpp"
#include "System/MonsterSystem.hpp"
#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;

wlEngine::EngineManager *engine = nullptr;

void initGameplaySystem();
int main() {

    engine = wlEngine::EngineManager::getwlEngine();
    wlEngine::Scene scene;

    engine->setScene(&scene);
    Shader::loadShader("stone_shader", "../Shader/stone_shader.vert", "../Shader/stone_shader.frag");
    Shader::loadShader("outline", "../Shader/outline.vert", "../Shader/outline.frag");
    scene.loadScene("../mainScene.json");
    initGameplaySystem();

    auto npc = scene.createGameObject(std::string("npc-test"), nullptr);
    npc->addComponent<Transform>(-480, 225, 0);
    npc->addComponent<Sprite>();
    auto animation = npc->addComponent<Animation>();
    animation->addAnimationFromAseprite("../resource/tasta/tasta.json", "../resource/tasta/tasta.png");
    animation->playAnimation("stand");
    npc->addComponent<NpcController>("trader-1");
    auto shape = PolygonShape(18, 20, 0, {0, -30});
    shape.leftPoint = {-12, -40};
    shape.rightPoint = {12, -40};
    auto trigidbody = npc->addComponent<TRigidbody>(&shape, BodyType::Dynamic);
    trigidbody->category = CONTACT_FILTER_INTERACTABLE;

    auto openGLGO = scene.createGameObject("OpenGLTest", nullptr);
    openGLGO->addComponent<OpenGLTest>();

    auto vl = scene.createGameObject("VL", nullptr);
    vl->addComponent<VolumetricLight>()->loadTexture("../resource/ray.jpg");

    vl->addComponent<Transform>(-480,225,0);

    engine->start();
}

void initGameplaySystem() {
    MonsterSystem::init();
    engine->addGameplaySystem(MonsterSystem::get());
}

