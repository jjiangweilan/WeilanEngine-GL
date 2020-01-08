#pragma once
#include "Texture.hpp"
#include <vector>

namespace wlEngine
{
namespace Graphics
{
class Mesh2D
{
public:
    Mesh2D();
    ~Mesh2D();
    void addTexture(const std::string &file);
    void addTexture(Texture *texture);
    void changeTexture(const int &loc, Texture *t);
    void clip(Rect *rect) const;

    const GLuint &getVAO() const;
    const std::vector<Texture *> *getTextures() const;
    const Rect *getClipRect() const;

private:
    std::vector<Texture *> m_textures;

    GLuint m_VAO = 0;
    GLuint m_VBO = 0;
    GLuint m_EBO = 0;

    mutable Rect m_rect;
    mutable float m_vertices[20] = {
        10.0f, 10.0f, 0.0f, 1.0f, 1.0f,   // top right
        10.0f, -10.0f, 0.0f, 1.0f, 0.0f,  // bottom right
        -10.0f, -10.0f, 0.0f, 0.0f, 0.0f, // bottom left
        10.0f, -10.0f, 0.0f, 0.0f, 1.0f   // top left
    };
};
} // namespace Graphics
} // namespace wlEngine