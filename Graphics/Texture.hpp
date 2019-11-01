#pragma once
#include "../Graphics/Shader.hpp"
#include "../Component/Struct/Clip.hpp"

#include <ft2build.h>
#include <glad/glad.h>
#include <stb_image.hpp>
#include FT_FREETYPE_H

namespace wlEngine
{
struct Texture
{
public:
    Texture() = default;
    Texture(const std::string &file);
    Texture(FT_GlyphSlot glyph);
    ~Texture();
    Texture *loadFromFile(const std::string &path);
    Texture *loadFromFTBitmap(FT_GlyphSlot bitmap_FT);
    void free();

    const unsigned int &getId() const;
    const std::string &getSourcePath() const;
    const int &getWidth() const;
    const int &getHeight() const;
    const int &getNRChannel() const;

private:
    unsigned int m_textureId = 0;
    std::string m_sourcePath;

    int m_width;
    int m_height;
    int m_nrChannel;
    
private:
    void load(unsigned char *data);
};
} // namespace wlEngine
