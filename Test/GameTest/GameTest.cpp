#include <gtest/gtest.h>
#include <Global/WeilanEngine.hpp>
#include "Scripts.hpp"

using namespace WeilanEngine;


TEST(GameTest, EngineStart)
{
    auto engine = WeilanEngine::EngineManager::GetWeilanEngine();
    Scene scene;

    auto renderNodes = scene.CreateGameObject("renderNodes");

    Graphics::Model::Add("nano", "C:/Users/weilan/Desktop/nano/nanosuit.obj");

    //hdr camera
    auto sceneCamera = scene.CreateGameObject("sceneCamera");
    sceneCamera->AddComponent<Transform>(0, 0, 10);
    auto camera3d = sceneCamera->AddComponent<Camera>();
	sceneCamera->AddComponent<CameraController>();

	auto model = scene.CreateGameObject("ModelTest");
    model->AddComponent<Transform>(0,0,0);
	auto modelM = model->AddComponent<Model>("nano");
    model->AddComponent<ModelUniformUpdate>();
    engine->SetScene(&scene);
    engine->Start();
}