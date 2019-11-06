#pragma once
#include <vector>
#include <glad/glad.h>
namespace wlEngine 
{
class Texture;
class Material
{
public:
    Material();

private:
    std::vector<Texture*> m_textures;
    GLuint m_fragmentShader;
};
}