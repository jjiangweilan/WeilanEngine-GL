#pragma once
#include "../Graphics/Texture.hpp"

#include <map>
#include <string>
#include <ft2build.h>
#include <SDL_mixer.h>
#include FT_FREETYPE_H
#include <json.hpp>

namespace wlEngine
{
struct TextInfo
{
    Texture text;
    FT_Face face;
};
using Json = nlohmann::json;
using TextureMap = std::map<std::string, Texture>;
using TextTextureMap = std::map<std::wstring, TextInfo>;
class ResourceManager
{
public:
    static void init();
    static ResourceManager *get() { return resourceManager; };
    ~ResourceManager();

    Texture *getTexture(const std::string &path);

    const TextureMap &getTextures();
    /* FreeType *************************************/
    TextInfo *getTextTexture(const wchar_t &wildCharacter, const int &pixelSizeWidth, const int &pixelSizeHeight);

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
    TextTextureMap textTextures;
    Texture *loadTexture(const std::string &path);

    Json npcJsonData;
    Json savedGameData;
    Json monsterData;

    FT_Library freeTypeLibrary;
    FT_Face face;

    std::map<std::string, Mix_Chunk *> audioChunks;
};
} // namespace wlEngine
