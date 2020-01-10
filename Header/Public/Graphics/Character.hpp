#pragma once
#include "Mesh2D.hpp"

namespace KuangyeEngine
{
namespace Graphics
{
class Character
{
public:
    /**
 * @brief Construct a new Character object
 * it seems that it makes more sense the character initialize the texture from the face
 * but since ResourceManager will store both the Character and Texture and it's
 * also the interface to getCharacter, the job is handled 
 * to ResourceManager instead
 * 
 * @param fact 
 * @param texture 
 */
    Character();
    Character(FT_Face face, Texture *texture);
    const GLuint getVAO() const;
    FT_Face getFace() const;
    const Texture *getTexture() const;
    const Mesh2D *getMesh() const;

private:
    Mesh2D m_mesh;
    FT_Face m_face;
};
} // namespace Graphics
} // namespace KuangyeEngine