#pragma once
#include "Graphics/Texture.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Character.hpp"

#include <unordered_map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_mixer.h>
#include <json.hpp>

namespace WeilanEngine
{
using Json = nlohmann::json;

//seperate texture and text texture because of they use different kinds of keys
using CharacterMap = std::unordered_map<std::wstring, Graphics::Character>;
class ResourceManager
{
public:
    static void init();
    static ResourceManager *Get() { return resourceManager; };
    ~ResourceManager();

    /* FreeType *************************************/
    Graphics::Character *GetCharacter(const wchar_t &wildCharacter, const int &pixelSizeWidth, const int &pixelSizeHeight);

    /* Get Data ***********************************/
    Json &getNpcJsonData();
    Json &getMonsterData();

    /* Audio */
    Mix_Chunk *getAudioChunk(const std::string &file);
    void FreeAudioChunk(const std::string &file);
    void FreeAudioChunk();

    const std::string& GetResourceDir();
    void SetResourceDir(std::string& dir);

    void SaveResourcePreferences();

private:
    std::string m_resourceDir;
    static ResourceManager *resourceManager;
    ResourceManager();
    CharacterMap m_characters;
    std::map<std::wstring, Graphics::Texture> m_textTextures;

    Json m_npcJsonData;
    Json m_savedGameData;
    Json m_monsterData;
    Json m_EngineConfig;

    FT_Library m_freeTypeLibrary;
    FT_Face m_face;

    std::map<std::string, Mix_Chunk *> m_audioChunks;

    //Resource Path
    void LocateResourceDirectory();
};
} // namespace WeilanEngine
