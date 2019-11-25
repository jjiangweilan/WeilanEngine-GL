#include "Animation.hpp"
#include "Sprite.hpp"
#include "../Time.hpp"
#include "../GameObject/Entity.hpp"
#include "../Manager/ResourceManager.hpp"

#include <json.hpp>
#include <fstream>
#include <iostream>
namespace wlEngine
{
using Json = nlohmann::json;

COMPONENT_DEFINATION(Component, Animation, 100);
COMPONENT_EDITABLE_DEF(Animation);

Animation::Animation(Entity *go) : Component(go), recursive(false), animationHasEnded(true)
{
    currentAnimation = nullptr;
    timeStamp = 0;
    currentFrame = 0;
}

Animation::Animation(Entity *go, void **args) : Component(go), currentAnimation(nullptr), timeStamp(0), currentFrame(0), recursive(false), animationHasEnded(true)
{
    if (args)
    {
        go->GetComponent<Sprite>()->getMesh()->clip(getCurrentClip());
    }
}

void Animation::addAnimationFromAseprite(const FilePath &animationJson, const FilePath &texturePath)
{
    std::ifstream jsonInput(animationJson);
    if (jsonInput.good())
    {
        std::string jsonStr((std::istreambuf_iterator<char>(jsonInput)), (std::istreambuf_iterator<char>()));
        Json animation = Json::parse(jsonStr);
        Json &frameTags = animation["meta"]["frameTags"];
        assert(frameTags.size() != 0 && "this aseprite file doesn't contain tag, which is required");
        for (int tagI = 0; tagI < frameTags.size(); tagI++)
        {
            std::string name = frameTags[tagI]["name"].get<std::string>();
            animations[name].atlas = Graphics::Texture::add(texturePath, texturePath);
            Json &frames = animation["frames"];

            int i = frameTags[tagI]["from"].get<int>();
            int frameEndIndex = frameTags[tagI]["to"].get<int>();

            for (; i <= frameEndIndex; i++)
            {
                Frame frame;
                frame.duration = frames[i]["duration"].get<float>() / 1000; // ms to s
                frame.rect.x = frames[i]["frame"]["x"];
                frame.rect.y = frames[i]["frame"]["y"];
                frame.rect.width = frames[i]["frame"]["w"];
                frame.rect.height = frames[i]["frame"]["h"];
                animations[name].frames.push_back(frame);
            }
        }
    }
}
void Animation::playAnimation(const std::string &name, bool recursive, bool timed)
{
    if (timed)
        if (Time::deltaTime == 0)
            return;
    animationHasEnded = false;
    this->recursive = recursive;
    currentAnimation = &animations[name];

    auto sprite = entity->GetComponent<Sprite>();
	sprite->changeTexture(0, currentAnimation->atlas);
    currentFrame = 0;
}

void Animation::pause() { currentAnimation = nullptr; }

Rect *Animation::getCurrentClip()
{
    if (currentAnimation == nullptr)
        return nullptr;

    return &currentAnimation->frames.at(currentFrame).rect;
}

bool Animation::isPlaying(const std::string &name) { return currentAnimation == &animations[name]; }

std::string Animation::getCurrentClipName() { return currentAnimation->name; }

bool Animation::hasEnded() { return animationHasEnded; }

int Animation::getCurrentFrame() { return currentFrame; }

} // namespace wlEngine
