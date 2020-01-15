#include "Graphics/Texture.hpp"
#include "Global/Settings.hpp"

namespace WeilanEngine
{
namespace Graphics
{
std::unordered_map<std::string, Texture> Texture::collection = std::unordered_map<std::string, Texture>();
Texture::Texture(const std::string &file, const TextureType &textureType) : m_type(textureType), m_textureId(0)
{
    loadFromFile(file, textureType);
}
Texture::Texture(const Texture &other)
{
    m_textureId = other.m_textureId;
    m_sourcePath = other.m_sourcePath;
    m_type = other.m_type;

    m_width = other.m_width;
    m_height = other.m_height;
    m_nrChannel = other.m_nrChannel;
}
Texture::Texture(Texture &&other)
{
    m_textureId = other.m_textureId;
    m_sourcePath = std::move(other.m_sourcePath);
    m_type = other.m_type;

    m_width = other.m_width;
    m_height = other.m_height;
    m_nrChannel = other.m_nrChannel;

    other.m_textureId = 0;
}

Texture &Texture::operator=(const Texture &other)
{
    m_textureId = other.m_textureId;
    m_sourcePath = other.m_sourcePath;
    m_type = other.m_type;

    m_width = other.m_width;
    m_height = other.m_height;
    m_nrChannel = other.m_nrChannel;
    return *this;
}
Texture &Texture::operator=(Texture &&other)
{
    m_textureId = other.m_textureId;
    m_sourcePath = std::move(other.m_sourcePath);
    m_type = other.m_type;

    m_width = other.m_width;
    m_height = other.m_height;
    m_nrChannel = other.m_nrChannel;

    other.m_textureId = 0;
    return *this;
}
Texture::Texture(FT_GlyphSlot glyph) : m_textureId(0)
{
    m_type = Diffuse;
    loadFromFTBitmap(glyph);
}
Texture *Texture::loadFromFTBitmap(FT_GlyphSlot glyph)
{
    Free();
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

Texture::Texture(const unsigned int &width,
                 const unsigned int &height,
                 const InternalFormat &internalFormat,
                 const DataFormat &format,
                 const DataType &type)
{
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(GL_TEXTURE_2D,0, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}
Texture *Texture::loadFromFile(const std::string &path, const TextureType &textureType)
{
    Free();
    m_sourcePath = path;
    m_type = textureType;

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

void Texture::Free()
{
    glDeleteTextures(1, &m_textureId);
    m_textureId = 0;
}

Texture::~Texture() { Free(); }
const unsigned int &Texture::GetId() const { return m_textureId; }
const std::string &Texture::getSourcePath() const { return m_sourcePath; }
const int &Texture::getWidth() const { return m_width; }
const int &Texture::getHeight() const { return m_height; }
const int &Texture::getNRChannel() const { return m_nrChannel; }
const TextureType &Texture::GetType() const { return m_type; }

Texture *Texture::get(const std::string &id)
{
    auto iter = collection.find(id);
    if (iter == collection.end())
        return nullptr;
    return &iter->second;
}

void Texture::remove(const std::string &id)
{
    collection.erase(id);
}
} // namespace Graphics
} // namespace WeilanEngine
