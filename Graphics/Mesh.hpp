#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <glad/glad.h>

namespace wlEngine
{
class Material;
struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 textureCoords;
    glm::vec3 tangent;
    glm::vec3 bitangent;
};

enum PrimitiveMeshType
{
    Cube, Sphere
};

class Mesh
{
public:
/**
 * @brief Construct a new Mesh object with a predefined mesh type
 * 
 * @param meshType 
 */
    Mesh(const PrimitiveMeshType& meshType);
    Mesh(std::vector<GLuint> &indices, std::vector<Vertex> &vertices);
    Mesh(std::vector<GLuint> &&indices, std::vector<Vertex> &&vertices);
    Mesh(const Mesh &mesh) = default;
    Mesh(Mesh &&mesh) noexcept;
    Mesh(aiMesh *mesh, Material* m = nullptr, const std::string& name="");

    /**
     * @brief load mesh's vertex data using assimp
     * 
     * @param mesh the assimp mesh
     */
    void loadFromAssimp(aiMesh *mesh);

    /**
     * @brief Set the Material object
     * 
     * @param material 
     */
    void setMaterial(Material *material) const;
	void setMaterial(const std::string& name) const;

    std::string name;
    ~Mesh(){};

private:
    std::vector<GLuint> m_indices;
    std::vector<Vertex> m_vertices;

    GLuint VAO;
    GLuint VBO;
    GLuint EBO;

    void setupMesh();

    mutable Material *m_material;

    friend class RenderSystem;
};
} // namespace wlEngine
