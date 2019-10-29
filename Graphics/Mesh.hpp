#pragma once
#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/material.h>

#include "Shader.hpp"

namespace wlEngine {
    struct Vertex{
        glm::vec3 position;
        glm::vec3 normal;       
        glm::vec2 textureCoords;
        glm::vec3 tangent;
        glm::vec3 bitangent;
    };

    enum class TextureType : unsigned int {
        None = aiTextureType_NONE,
        Diffuse = aiTextureType_DIFFUSE,
        Specular = aiTextureType_SPECULAR,
        Ambient = aiTextureType_AMBIENT,
        Normal = aiTextureType_NORMALS,
        Shininess = aiTextureType_SHININESS,
    };

    struct Texture3D {
        GLuint id;
        std::string type;
        std::string resourcePath;
    };

    class Mesh {
    public:
        Mesh(std::vector<Texture3D>& textures, std::vector<GLuint>& indices, std::vector<Vertex>& vertices);
        Mesh(std::vector<Texture3D>&& textures, std::vector<GLuint>&& indices, std::vector<Vertex>&& vertices);
		Mesh(const Mesh& mesh) = default;
		Mesh(Mesh&& mesh) noexcept;

		~Mesh() {};

    private:
        std::vector<Texture3D> textures;
        std::vector<GLuint> indices;
        std::vector<Vertex> vertices;

        GLuint VAO;
        GLuint VBO;
        GLuint EBO;

        void setupMesh();

        friend class RenderSystem;
    };
}
