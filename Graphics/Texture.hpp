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
    ~Texture();
    Texture *loadFromFile(const std::string &path);
    Texture *loadFromFTBitmap(const FT_GlyphSlot &bitmap_FT);
    void free();

    void clip(Rect *, bool subData = true) const;
    void clipReset();
    void clipAndScaleFreeTypeBitmap(Rect *);

    const unsigned int &getId() const;
    const std::string &getSourcePath() const;
    const int &getWidth() const;
    const int &getHeight() const;
    const int &getNRChannel() const;
    const Rect &getClipRect() const;
    const float &getNormalizationParam() const;
    const GLuint &getVAO() const;
    const GLuint &getVBO() const;
    const GLuint &getEBO() const;

private:
    unsigned int m_textureId = 0;
    std::string m_sourcePath;

    int m_width;
    int m_height;
    int m_nrChannel;
    
    float m_normalizationParam;

    //opengl
    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;

	mutable Rect m_rect;
    mutable float m_vertices[20] = {
        0.0f, 0.0f, 0.0f, 1.0f, 1.0f, // top right
        0.0f, 0.0f, 0.0f, 1.0f, 0.0f, // bottom right
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, // bottom left
        0.0f, 0.0f, 0.0f, 0.0f, 1.0f  // top left
    };

private:
    void load(unsigned char *data);
};
} // namespace wlEngine
