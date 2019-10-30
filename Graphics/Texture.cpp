#include "Texture.hpp"

#include "../Settings.hpp"

namespace wlEngine
{
Texture::Texture(const std::string &file)
{
    loadFromFile(file);
}
Texture *Texture::loadFromFTBitmap(const FT_GlyphSlot &glyph)
{
    free();
    m_sourcePath = "load from FTBitmap";
    m_width = glyph->bitmap.width;
    m_height = glyph->bitmap.rows;

#if SETTINGS_GAME_DIMENSION == 0
    float width = m_width;
    float height = m_height;
#else
    assert(0 && "no 3d considered");
    float width = m_rect->width / noralizationPara;
    float height = m_rect->height / m_normalizationParam;
#endif

    m_vertices[0] = glyph->bitmap_left + glyph->bitmap.width;
    m_vertices[1] = glyph->bitmap_top;
    m_vertices[5] = glyph->bitmap_left + glyph->bitmap.width;
    m_vertices[6] = -int(glyph->bitmap.rows - glyph->bitmap_top);
    m_vertices[10] = glyph->bitmap_left;
    m_vertices[11] = -int(glyph->bitmap.rows - glyph->bitmap_top);
    m_vertices[15] = glyph->bitmap_left;
    m_vertices[16] = glyph->bitmap_top;

    //top right
    m_vertices[8] = 1;
    m_vertices[9] = 1;

    //bottom right
    m_vertices[3] = 1;
    m_vertices[4] = 0;

    //bottom left
    m_vertices[18] = 0;
    m_vertices[19] = 0;

    //top left
    m_vertices[13] = 0;
    m_vertices[14] = 1;

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // Generate texture
    glGenTextures(1, &m_textureId);
    glBindTexture(GL_TEXTURE_2D, m_textureId);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RED,
        width,
        height,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        glyph->bitmap.buffer);
    // Set texture options
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

    glBindTexture(GL_TEXTURE_2D, 0);
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
    m_normalizationParam = m_width > m_height ? m_width : m_height; // for 3d

    Rect m_rect(0, 0, m_width, m_height);
    clip(&m_rect, false);
    unsigned int glColorChannel = m_nrChannel == 4 ? GL_RGBA : GL_RGB;

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &m_VAO);
    glGenBuffers(1, &m_VBO);
    glGenBuffers(1, &m_EBO);
    glBindVertexArray(m_VAO);
    glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(m_vertices), m_vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);

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
    glDeleteBuffers(1, &m_VBO);
    glDeleteBuffers(1, &m_EBO);
    glDeleteBuffers(1, &m_VAO);

    m_textureId = 0;
    m_VBO = 0;
    m_EBO = 0;
    m_VAO = 0;
}

void Texture::clip(Rect *m_rect, bool subData) const
{
    this->m_rect = *m_rect;
#if SETTINGS_GAME_DIMENSION == 0
    float width = m_rect->width;
    float height = m_rect->height;
#else
    float width = m_rect->width / noralizationPara;
    float height = m_rect->height / m_normalizationParam;
#endif

    m_vertices[0] = width / 2;
    m_vertices[1] = height / 2;
    m_vertices[5] = width / 2;
    m_vertices[6] = -height / 2;
    m_vertices[10] = -width / 2;
    m_vertices[11] = -height / 2;
    m_vertices[15] = -width / 2;
    m_vertices[16] = height / 2;

    //top right
    m_vertices[3] = (float)(m_rect->x + m_rect->width) / m_width;
    m_vertices[4] = (float)(m_rect->y + m_rect->height) / m_height;

    //bottom right
    m_vertices[8] = (float)(m_rect->x + m_rect->width) / m_width;
    m_vertices[9] = (float)(m_rect->y) / m_height;

    //bottom left
    m_vertices[13] = (float)(m_rect->x) / m_width;
    m_vertices[14] = (float)(m_rect->y) / m_height;

    //top left
    m_vertices[18] = (float)(m_rect->x) / m_width;
    m_vertices[19] = (float)(m_rect->y + m_rect->height) / m_height;

    if (subData)
    {
        //update to opengl
        glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(m_vertices), m_vertices);
    }
}

void Texture::clipAndScaleFreeTypeBitmap(Rect *m_rect)
{
}

void Texture::clipReset()
{
    Rect m_rect(0, 0, m_width, m_height);
    clip(&m_rect, true);
}

Texture::~Texture() { free(); }
const unsigned int &Texture::getId() const { return m_textureId; }
const std::string &Texture::getSourcePath() const { return m_sourcePath; }
const int &Texture::getWidth() const { return m_width; }
const int &Texture::getHeight() const { return m_height; }
const int &Texture::getNRChannel() const { return m_nrChannel; }
const Rect &Texture::getClipRect() const { return m_rect; }
const float &Texture::getNormalizationParam() const { return m_normalizationParam; }
const GLuint &Texture::getVAO() const { return m_VAO; }
const GLuint &Texture::getVBO() const { return m_VBO; }
const GLuint &Texture::getEBO() const { return m_EBO; }
} // namespace wlEngine
