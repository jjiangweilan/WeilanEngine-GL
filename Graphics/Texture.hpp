#pragma once
#include "../Graphics/Shader.hpp"
#include "../Component/Struct/Clip.hpp"

#include <ft2build.h>
#include <glad/glad.h>
#include <stb_image.hpp>
#include <assimp/material.h>
#include <unordered_map>
#include FT_FREETYPE_H

namespace wlEngine
{
namespace Graphics
{
enum TextureType
{
    None = aiTextureType_NONE,
    Diffuse = aiTextureType_DIFFUSE,
    Specular = aiTextureType_SPECULAR,
    Ambient = aiTextureType_AMBIENT,
    Emissive = aiTextureType_EMISSIVE,
    Height = aiTextureType_HEIGHT,
    Normals = aiTextureType_NORMALS,
    Shininess = aiTextureType_SHININESS,
    Opacity = aiTextureType_OPACITY,
    Displacement = aiTextureType_DISPLACEMENT,
    Lightmap = aiTextureType_LIGHTMAP,
    Reflection = aiTextureType_REFLECTION,
    Unknown = aiTextureType_UNKNOWN,
};

class Texture
{
public:
    Texture() = default;
    Texture(const std::string &file, const TextureType &type = TextureType::Diffuse);
    Texture(FT_GlyphSlot glyph);
    ~Texture();
    Texture *loadFromFile(const std::string &path, const TextureType &textureType = TextureType::Diffuse);
    Texture *loadFromFTBitmap(FT_GlyphSlot bitmap_FT);
    void free();

    const unsigned int &getId() const;
    const std::string &getSourcePath() const;
    const int &getWidth() const;
    const int &getHeight() const;
    const int &getNRChannel() const;
    const TextureType &GetType() const;

private:
    unsigned int m_textureId = 0;
    std::string m_sourcePath;
    TextureType m_type;

    int m_width;
    int m_height;
    int m_nrChannel;

    void load(unsigned char *data);

    /* Static ----*/
public:
    static Texture *get(const std::string &id);
    template <typename... Args>
    static Texture *add(const std::string &id, Args &&... args);
    static void remove(const std::string &id);

private:
    static std::unordered_map<std::string, Texture> collection;
};

template <typename... Args>
Texture *Texture::add(const std::string &id, Args &&... args)
{
	auto has = collection.find(id);
	if (has != collection.end()) return &has->second;

    auto pair = collection.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...));
	return &pair.first->second;
}
} // namespace Graphics
} // namespace wlEngine
