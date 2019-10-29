#include <string>
#include "UserInterface.hpp"
#include "EngineManager.hpp"
#include "System/RenderSystem.hpp"
#include "Manager/ResourceManager.hpp"
#include "Component/NpcController.hpp"
#include "Component/Transform.hpp"
#include "Component/Text.hpp"
#include "Component/Sprite.hpp"
#include "GameObject/Entity.hpp"

namespace wlEngine {
Entity *UserInterface::showDialog(NpcController *npcController) {
    auto engineManager = EngineManager::getwlEngine();
    auto renderSystem = RenderSystem::get();
    auto scene = engineManager->getCurrentScene();
    auto dialog = scene->createGameObject("Dialog", scene->getCamera());

    auto tranform = dialog->addComponent<Transform>();
    auto sceneSize = renderSystem->getSceneSize();
    tranform->setLocalPosition({sceneSize.x / 2.0, 100, 10000});
    auto background = dialog->addComponent<Sprite>("../resource/tasta/dialog-background.png");

    auto text = scene->createGameObject("Dialog-text", dialog);
    text->addComponent<Transform>()->setLocalPosition({-354, 70, 1});

    auto &behavior = npcController->behaviorList[npcController->todaysBehaviorIndex];
    auto &str = behavior.conversation[0].options.begin()->second;

    text->addComponent<Text>()->loadText(L"Hi, that boss from the grocery store told me this looks like a milestone!!", 708, 40, 28, 28);
    return dialog;
}

void UserInterface::switchToUIMode() {}
} // namespace wlEngine