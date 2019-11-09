#pragma once
#include "../Graphics/Texture.hpp"
#include "../Graphics/Material.hpp"
#include "../Graphics/Character.hpp"

#include <unordered_map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H
#include <SDL_mixer.h>
#include <json.hpp>

namespace wlEngine
{
using Json = nlohmann::json;

//seperate texture and text texture because of they use different kinds of keys
using TextureMap = std::unordered_map<std::string, Texture>;
using CharacterMap = std::unordered_map<std::wstring, Character>;
using MaterialMap = std::unordered_map<std::string, Material>;
class ResourceManager
{
public:
    static void init();
    static ResourceManager *get() { return resourceManager; };
    ~ResourceManager();

    Texture *getTexture(const std::string &path, const TextureType& type = TextureType::Diffuse);
    Material* getMaterial(const std::string& name);

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
    TextureMap m_textures;
    CharacterMap m_characters;
    MaterialMap m_materials;
    std::map<std::wstring, Texture> m_textTextures;
    Texture *loadTexture(const std::string &path, const TextureType &type = TextureType::Diffuse);

    Json m_npcJsonData;
    Json m_savedGameData;
    Json m_monsterData;

    FT_Library m_freeTypeLibrary;
    FT_Face m_face;

    std::map<std::string, Mix_Chunk *> m_audioChunks;
};
} // namespace wlEngine
