#include <gtest/gtest.h>
#include <Global/KuangyeEngine.hpp>
#include "Scripts.hpp"

using namespace KuangyeEngine;
TEST(GameTest, EngineStart)
{
    auto engine = KuangyeEngine::EngineManager::GetKuangyeEngine();
    Scene scene;

    auto renderNodes = scene.CreateGameObject("renderNodes");
	
    //hdr camera
    auto sceneCamera = scene.CreateGameObject("sceneCamera");
    sceneCamera->AddComponent<Transform>(0, 3, 12);
    auto camera3d = sceneCamera->AddComponent<Camera3D>();
    auto sceneRenderNode = sceneCamera->AddComponent<RenderContext>();
    sceneRenderNode->GenFramebuffer();
	sceneRenderNode->AttachTexture2D(PredefinedAttachmentType::HDR);
	sceneRenderNode->AttachTexture2D(PredefinedAttachmentType::Depth);
    sceneCamera->AddComponent<CameraRenderUpdate>();

    scene.SetMainCamera(sceneCamera);
    RenderSystem::Get()->SetOutputRenderNode(sceneRenderNode);

    engine->SetScene(&scene);
    engine->Start();
}