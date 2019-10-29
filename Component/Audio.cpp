#include "Audio.hpp"
#include <SDL_audio.h>
#include <iostream>
#include "../GameObject/Entity.hpp"
#include "../Manager/ResourceManager.hpp"

namespace wlEngine {
    COMPONENT_DEFINATION(Component, Audio, 32);
    Audio::Audio(Entity* entity) : Component(entity) {
    }

    void Audio::play(const std::string& file) {
        auto chunk = ResourceManager::get()->getAudioChunk(file);

        if (Mix_PlayChannel(-1, chunk, 0) == -1) {
            std::cerr << "playing audio chunk failed: " << entity->name << " " << file << std::endl;
        }
    }
}