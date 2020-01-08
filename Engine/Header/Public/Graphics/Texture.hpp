#pragma once
#include "Graphics/Shader.hpp"
#include "Component/Clip.hpp"

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
	enum InternalFormat
    {
        RGBAInternal = GL_RGBA,
        RGBInternal = GL_RGB,
        RGB8 = GL_RGB8,
        RGBA8 = GL_RGBA8,
        RGBA16F = GL_RGBA16F,
        RGB16F = GL_RGB16F,
        DepthComponent = GL_DEPTH_COMPONENT,
        DepthComponent16 = GL_DEPTH_COMPONENT16,
        DepthComponent24 = GL_DEPTH_COMPONENT24,
        DepthComponent32 = GL_DEPTH_COMPONENT32,
        StencilIndex = GL_STENCIL_INDEX,
        StencilIndex1 = GL_STENCIL_INDEX1,
        StencilIndex4 = GL_STENCIL_INDEX4,
        StencilIndex8 = GL_STENCIL_INDEX8,
        StencilIndex16 = GL_STENCIL_INDEX16,
        DepthStencilInternal = GL_DEPTH_STENCIL
    };
    enum DataFormat
    {
        Red = GL_RED,
        RG = GL_RG,
        RGB = GL_RGB,
        BGR = GL_BGR,
        RGBA = GL_RGBA,
        BGRA = GL_BGRA,
        RedInteger  =GL_RED_INTEGER,
        RGInteger = GL_RG_INTEGER,
        RGBInteger = GL_RGB_INTEGER,
        BGRInteger = GL_BGR_INTEGER,
        RGBAInteger = GL_RGBA_INTEGER,
        BGRAInteger = GL_BGRA_INTEGER,
        Stencil = GL_STENCIL_INDEX,
        Depth = GL_DEPTH_COMPONENT,
        DepthStencil = GL_DEPTH_STENCIL
    };
    enum DataType
    {
        UnsignedByte = GL_UNSIGNED_BYTE,
        Byte = GL_BYTE,
        UnsignedShort = GL_UNSIGNED_SHORT,
        Short = GL_SHORT,
        UnsignedInt = GL_UNSIGNED_INT,
        Int = GL_INT,
        HalfFloat = GL_HALF_FLOAT,
        Float = GL_FLOAT,
        UnsignedByte332 = GL_UNSIGNED_BYTE_3_3_2
       // GL_UNSIGNED_BYTE_2_3_3_REV,
       // GL_UNSIGNED_SHORT_5_6_5,
       // GL_UNSIGNED_SHORT_5_6_5_REV,
       // GL_UNSIGNED_SHORT_4_4_4_4,
       // GL_UNSIGNED_SHORT_4_4_4_4_REV,
       // GL_UNSIGNED_SHORT_5_5_5_1,
       // GL_UNSIGNED_SHORT_1_5_5_5_REV,
       // GL_UNSIGNED_INT_8_8_8_8,
       // GL_UNSIGNED_INT_8_8_8_8_REV,
       // GL_UNSIGNED_INT_10_10_10_2,
       // and GL_UNSIGNED_INT_2_10_10_10_REV
    };
    Texture() = default;
    Texture(const unsigned int &width,
            const unsigned int &height,
            const InternalFormat &internalFormat,
            const DataFormat &format,
            const DataType &type);
    Texture(const Texture& other);
    Texture(Texture&& other);

    Texture& operator=(const Texture& other);
    Texture& operator=(Texture&& other);

    Texture(const std::string &file, const TextureType &type = TextureType::Diffuse);
    Texture(FT_GlyphSlot glyph);
    ~Texture();
    Texture *loadFromFile(const std::string &path, const TextureType &textureType = TextureType::Diffuse);
    Texture *loadFromFTBitmap(FT_GlyphSlot bitmap_FT);
    void Free();

    const unsigned int &GetId() const;
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
    static Texture *Add(const std::string &id, Args &&... args);
    static void remove(const std::string &id);

private:
    static std::unordered_map<std::string, Texture> collection;
};

template <typename... Args>
Texture *Texture::Add(const std::string &id, Args &&... args)
{
    auto has = collection.find(id);
    if (has != collection.end())
        return &has->second;

    auto pair = collection.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...));
    return &pair.first->second;
}
} // namespace Graphics
} // namespace wlEngine
