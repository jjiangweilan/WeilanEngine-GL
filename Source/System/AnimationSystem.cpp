#include "System/AnimationSystem.hpp"

#include "Manager/EngineManager.hpp"
#include "GameObject/Entity.hpp"
#include "Component/Sprite.hpp"
#include "Component/Animation.hpp"
#include "Global/Time.hpp"

namespace KuangyeEngine
{
SYSTEM_DEFINATION(AnimationSystem);
AnimationSystem::AnimationSystem()
{
}

void AnimationSystem::update()
{
    auto currentScene = EngineManager::GetKuangyeEngine()->getCurrentScene();
    for (auto &c : Animation::collection)
    {
        if (!c->entity->IsEnable() || c->entity->GetScene() != currentScene)
            continue;
        updateClip(c);
    }
}

void AnimationSystem::updateClip(Animation *a)
{
    if (a->animationHasEnded)
        return;
    a->timeStamp += Time::deltaTime;
    if (a->timeStamp >= a->currentAnimation->frames.at(a->currentFrame).duration)
    {
        a->timeStamp = 0;
        a->currentFrame++;
        if (a->currentFrame == (int)a->currentAnimation->frames.size() && !a->recursive)
        { // AnimationSystem's update is called before RenderSystem (in the same frame, the last frame of the animation is rendered after the animation is set to true)
            a->animationHasEnded = true;
        }
        a->currentFrame %= a->currentAnimation->frames.size();
    }
}

} // namespace KuangyeEngine
