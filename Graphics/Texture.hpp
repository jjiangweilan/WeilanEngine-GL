#pragma once
#include "../Graphics/Shader.hpp"
#include "../Component/Struct/Clip.hpp"

#include <ft2build.h>
#include <glad/glad.h>
#include <stb_image.hpp>
#include <assimp/material.h>
#include FT_FREETYPE_H

namespace wlEngine
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

struct Texture
{
public:
    Texture() = default;
    Texture(const std::string &file, const TextureType& type=TextureType::Diffuse);
    Texture(FT_GlyphSlot glyph);
    ~Texture();
    Texture *loadFromFile(const std::string &path, const TextureType& textureType = TextureType::Diffuse);
    Texture *loadFromFTBitmap(FT_GlyphSlot bitmap_FT);
    void free();

    const unsigned int &getId() const;
    const std::string &getSourcePath() const;
    const int &getWidth() const;
    const int &getHeight() const;
    const int &getNRChannel() const;
    const TextureType& getType() const;

private:
    unsigned int m_textureId = 0;
    std::string m_sourcePath;
    TextureType m_type;

    int m_width;
    int m_height;
    int m_nrChannel;

private:
    void load(unsigned char *data);
};
} // namespace wlEngine
