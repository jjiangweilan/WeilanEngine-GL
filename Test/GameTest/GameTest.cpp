#include <gtest/gtest.h>
#include <Global/WeilanEngine.hpp>
#include "Scripts.hpp"

using namespace WeilanEngine;
TEST(GameTest, EngineStart)
{
    auto engine = WeilanEngine::EngineManager::GetWeilanEngine();
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