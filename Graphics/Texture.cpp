#include "Texture.hpp"

#include "../Settings.hpp"

namespace wlEngine
{
Texture::Texture(const std::string &file)
{
    loadFromFile(file);
}
Texture::Texture(FT_GlyphSlot glyph)
{
    loadFromFTBitmap(glyph);
}
Texture *Texture::loadFromFTBitmap(FT_GlyphSlot glyph)
{
    free();
    m_sourcePath = "load from FTBitmap";
    m_width = glyph->bitmap.width;
    m_height = glyph->bitmap.rows;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Generate texture
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D, 0,
                 GL_RED, m_width,
                 m_height, 0,
                 GL_RED, GL_UNSIGNED_BYTE,
                 glyph->bitmap.buffer);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    return this;
}

Texture *Texture::loadFromFile(const std::string &path)
{
    free();
    m_sourcePath = path;

    unsigned char *imageData = stbi_load(path.c_str(), &m_width, &m_height, &m_nrChannel, 0);
    load(imageData);

    return this;
}

void Texture::load(unsigned char *imageData)
{
    unsigned int glColorChannel = m_nrChannel == 4 ? GL_RGBA : GL_RGB;

    if (imageData)
    {
        glGenTextures(1, &m_textureId);
        glBindTexture(GL_TEXTURE_2D, m_textureId);
        glTexImage2D(GL_TEXTURE_2D, 0, glColorChannel, m_width, m_height, 0, glColorChannel, GL_UNSIGNED_BYTE, imageData);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glGenerateMipmap(GL_TEXTURE_2D);

        stbi_image_free(imageData);
    }
#ifdef DEBUG
    else
        std::cerr << "Texture: no image data source: " << m_sourcePath << std::endl;
#endif
}

void Texture::free()
{
    glDeleteTextures(1, &m_textureId);
    m_textureId = 0;
}

Texture::~Texture() { free(); }
const unsigned int &Texture::getId() const { return m_textureId; }
const std::string &Texture::getSourcePath() const { return m_sourcePath; }
const int &Texture::getWidth() const { return m_width; }
const int &Texture::getHeight() const { return m_height; }
const int &Texture::getNRChannel() const { return m_nrChannel; }

} // namespace wlEngine
