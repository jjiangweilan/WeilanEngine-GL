#pragma once
#include "../Graphics/Texture.hpp"
#include "../Graphics/Character.hpp"

#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_mixer.h>
#include <json.hpp>

namespace wlEngine
{
using Json = nlohmann::json;

//seperate texture and text texture because of they use different kinds of keys
using TextureMap = std::map<std::string, Texture>;
using CharacterMap = std::map<std::wstring, Character>;
class ResourceManager
{
public:
    static void init();
    static ResourceManager *get() { return resourceManager; };
    ~ResourceManager();

    Texture *getTexture(const std::string &path);

    const TextureMap &getTextures();
    /* FreeType *************************************/
    Character *getCharacter(const wchar_t &wildCharacter, const int &pixelSizeWidth, const int &pixelSizeHeight);

    /* Get Data ***********************************/
    Json &getNpcJsonData();
    Json &getMonsterData();

    /* Audio */
    Mix_Chunk *getAudioChunk(const std::string &file);
    void freeAudioChunk(const std::string &file);
    void freeAudioChunk();

private:
    static ResourceManager *resourceManager;
    ResourceManager();
    TextureMap textures;
    CharacterMap characters;
    std::map<std::wstring, Texture> textTextures;
    Texture *loadTexture(const std::string &path);

    Json npcJsonData;
    Json savedGameData;
    Json monsterData;

    FT_Library freeTypeLibrary;
    FT_Face face;

    std::map<std::string, Mix_Chunk *> audioChunks;
};
} // namespace wlEngine
