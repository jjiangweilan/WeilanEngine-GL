#pragma once
#include "Shader.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/material.h>

namespace wlEngine
{
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

class Mesh
{
public:
    Mesh(std::vector<Texture*> &textures, std::vector<GLuint> &indices, std::vector<Vertex> &vertices);
    Mesh(std::vector<Texture*> &&textures, std::vector<GLuint> &&indices, std::vector<Vertex> &&vertices);
    Mesh(const Mesh &mesh) = default;
    Mesh(Mesh &&mesh) noexcept;

    ~Mesh(){};

private:
    std::vector<Texture*> textures;
    std::vector<GLuint> indices;
    std::vector<Vertex> vertices;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    void setupMesh();

    friend class RenderSystem;
};
} // namespace wlEngine
