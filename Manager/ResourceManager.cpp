#include "ResourceManager.hpp"

#include "../Utility/Utility.hpp"

#include <stb_image.hpp>
#include <algorithm>
#include <locale.h>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <stb_image.hpp>
#include <sstream>

namespace wlEngine
{
ResourceManager *ResourceManager::resourceManager = nullptr;
ResourceManager::~ResourceManager()
{
    FT_Done_Face(face);
    FT_Done_FreeType(freeTypeLibrary);
    freeAudioChunk();
    Mix_CloseAudio();
}

ResourceManager::ResourceManager()
{
    //load text resource
    FT_Error error = FT_Init_FreeType(&freeTypeLibrary);
    if (error)
    {
        std::cerr << "FT Library Init Error\n";
    }

    error = FT_New_Face(freeTypeLibrary, "../wlEngine/etc/fonts/Zpix.ttf", 0, &face);

    if (error)
    {
        std::cerr << "FT Face Init Error\n";
    }

    //npc json data
    std::ifstream ifs;
    ifs.open("../NpcData/NpcJsonData.json");
    if (ifs)
    {
        std::ostringstream oss;
        oss << ifs.rdbuf();
        npcJsonData = Json::parse(oss.str());
    }
    ifs.close();

    ifs.close();
}
void ResourceManager::init()
{
    resourceManager = new ResourceManager();
    setlocale(LC_ALL, "");
}

Texture *ResourceManager::loadTexture(const std::string &path)
{
    return textures[path].loadFromFile(path);
}

Texture *ResourceManager::getTexture(const std::string &path)
{
    auto t = textures.find(path);
    if (t == textures.end())
    {
        return loadTexture(path);
    }
    return &t->second;
}

const TextureMap &ResourceManager::getTextures()
{
    return textures;
}

Character *ResourceManager::getCharacter(const wchar_t &wideCharacter, const int &pixelSizeWidth, const int &pixelSizeHeight)
{
    std::wstring id = std::wstring(1, wideCharacter) + L"_" + std::to_wstring(pixelSizeWidth) + L"_" + std::to_wstring(pixelSizeHeight); // IMPROVE: we can write this as a struct to improve performance
    auto iter = characters.find(id);
    if (iter != characters.end())
        return &characters[id];

    FT_UInt glyph_index = FT_Get_Char_Index(face, (FT_ULong)wideCharacter);
    FT_Error error = FT_Set_Pixel_Sizes(
        face,
        pixelSizeWidth, pixelSizeHeight);
    error = FT_Load_Glyph(face, glyph_index, FT_LOAD_DEFAULT);
    error = FT_Render_Glyph(face->glyph, FT_RENDER_MODE_NORMAL);

    auto characterTexture = textTextures[id].loadFromFTBitmap(face->glyph);
    characters[id] = Character(face, characterTexture);
    return &characters[id];
}

Json &ResourceManager::getNpcJsonData()
{
    return npcJsonData;
}

Json &ResourceManager::getMonsterData()
{
    return monsterData;
}

Mix_Chunk *ResourceManager::getAudioChunk(const std::string &file)
{
    auto find = audioChunks.find(file);
    if (find != audioChunks.end())
    {
        return find->second;
    }

    auto rlt = Mix_LoadWAV(file.data());
    if (rlt)
    {
        audioChunks[file] = rlt;
    }
    else
        std::cerr << "audio chunk loads failed: " << file << std::endl;
    return rlt;
}

void ResourceManager::freeAudioChunk(const std::string &file)
{
    auto chunk = audioChunks[file];
    if (chunk)
    {
        Mix_FreeChunk(chunk);
        audioChunks.erase(file);
    }
    else
    {
        std::cerr << "there is no audio chunk named " << file << std::endl;
    }
}

void ResourceManager::freeAudioChunk()
{
    for (auto pair : audioChunks)
    {
        Mix_FreeChunk(pair.second);
    }
    audioChunks.clear();
}
} // namespace wlEngine
