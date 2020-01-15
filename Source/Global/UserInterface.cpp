#include <string>
#include "Global/UserInterface.hpp"
#include "Manager/EngineManager.hpp"
#include "System/RenderSystem.hpp"
#include "Manager/ResourceManager.hpp"
#include "Component/NpcController.hpp"
#include "Component/Transform.hpp"
#include "Component/Text.hpp"
#include "Component/Sprite.hpp"
#include "GameObject/Entity.hpp"

namespace WeilanEngine
{
Entity *UserInterface::showDialog(NpcController *npcController)
{
    auto engineManager = EngineManager::GetWeilanEngine();
    auto renderSystem = RenderSystem::Get();
    auto scene = engineManager->getCurrentScene();
    auto dialog = scene->CreateGameObject("Dialog", scene->getCamera());

    auto tranform = dialog->AddComponent<Transform>();
    auto sceneSize = renderSystem->GetSceneSize();
    tranform->setLocalPosition({sceneSize.x / 2.0, 100, 10000});
    auto background = dialog->AddComponent<Sprite>("../resource/tasta/dialog-background.png");

    auto text = scene->CreateGameObject("Dialog-text", dialog);
    text->AddComponent<Transform>()->setLocalPosition({-354, 70, 1});

    auto &behavior = npcController->behaviorList[npcController->todaysBehaviorIndex];
    auto &str = behavior.conversation[0].options.begin()->second;

    text->AddComponent<Text>()->loadText(L"Hi, that boss from the grocery store told me this looks like a milestone!!", 708, 40, 28, 28);
    return dialog;
}

void UserInterface::switchToUIMode() {}
} // namespace WeilanEngine