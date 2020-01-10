#include "Graphics/Character.hpp"
#include "Manager/ResourceManager.hpp"

namespace KuangyeEngine
{
namespace Graphics
{
Character::Character() : m_mesh(), m_face()
{
    std::cout << "called" << std::endl;
}

Character::Character(FT_Face face, Texture *characterTexture)
{
    m_face = face;
    auto glyph = face->glyph;

    m_mesh.addTexture(characterTexture);
}

FT_Face Character::getFace() const
{
    return m_face;
}

const Mesh2D *Character::getMesh() const
{
    return &m_mesh;
}
} // namespace Graphics
} // namespace KuangyeEngine