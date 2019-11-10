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
    FT_Done_Face(m_face);
    FT_Done_FreeType(m_freeTypeLibrary);
    freeAudioChunk();
    Mix_CloseAudio();
}

ResourceManager::ResourceManager()
{
    //load text resource
    FT_Error error = FT_Init_FreeType(&m_freeTypeLibrary);
    if (error)
    {
        std::cerr << "FT Library Init Error\n";
    }

    error = FT_New_Face(m_freeTypeLibrary, "../wlEngine/etc/fonts/Zpix.ttf", 0, &m_face);

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
        m_npcJsonData = Json::parse(oss.str());
    }
    ifs.close();

    ifs.close();
}
void ResourceManager::init()
{
    resourceManager = new ResourceManager();
    setlocale(LC_ALL, "");
}

Graphics::Character *ResourceManager::getCharacter(const wchar_t &wideCharacter, const int &pixelSizeWidth, const int &pixelSizeHeight)
{
    std::wstring id = std::wstring(1, wideCharacter) + L"_" + std::to_wstring(pixelSizeWidth) + L"_" + std::to_wstring(pixelSizeHeight); // IMPROVE: we can write this as a struct to improve performance
    auto iter = m_characters.find(id);
    if (iter != m_characters.end())
        return &m_characters[id];

    FT_UInt glyph_index = FT_Get_Char_Index(m_face, (FT_ULong)wideCharacter);
    FT_Error error = FT_Set_Pixel_Sizes(
        m_face,
        pixelSizeWidth, pixelSizeHeight);
    error = FT_Load_Glyph(m_face, glyph_index, FT_LOAD_DEFAULT);
    error = FT_Render_Glyph(m_face->glyph, FT_RENDER_MODE_NORMAL);

    auto characterTexture = m_textTextures[id].loadFromFTBitmap(m_face->glyph);
    m_characters[id] = Graphics::Character(m_face, characterTexture);
    return &m_characters[id];
}

Json &ResourceManager::getNpcJsonData()
{
    return m_npcJsonData;
}

Json &ResourceManager::getMonsterData()
{
    return m_monsterData;
}

Mix_Chunk *ResourceManager::getAudioChunk(const std::string &file)
{
    auto find = m_audioChunks.find(file);
    if (find != m_audioChunks.end())
    {
        return find->second;
    }

    auto rlt = Mix_LoadWAV(file.data());
    if (rlt)
    {
        m_audioChunks[file] = rlt;
    }
    else
        std::cerr << "audio chunk loads failed: " << file << std::endl;
    return rlt;
}

void ResourceManager::freeAudioChunk(const std::string &file)
{
    auto chunk = m_audioChunks[file];
    if (chunk)
    {
        Mix_FreeChunk(chunk);
        m_audioChunks.erase(file);
    }
    else
    {
        std::cerr << "there is no audio chunk named " << file << std::endl;
    }
}

void ResourceManager::freeAudioChunk()
{
    for (auto pair : m_audioChunks)
    {
        Mix_FreeChunk(pair.second);
    }
    m_audioChunks.clear();
}

} // namespace wlEngine
