#include "Component/Audio.hpp"
#include "GameObject/Entity.hpp"
#include "Manager/ResourceManager.hpp"

#include <SDL_audio.h>
#include <iostream>

namespace KuangyeEngine
{
COMPONENT_DEFINATION(Component, Audio, 32);
Audio::Audio(Entity *entity) : Component(entity)
{
}

void Audio::play(const std::string &file)
{
    auto chunk = ResourceManager::Get()->getAudioChunk(file);

    if (Mix_PlayChannel(-1, chunk, 0) == -1)
    {
        std::cerr << "playing audio chunk failed: " << entity->name << " " << file << std::endl;
    }
}
} // namespace KuangyeEngine