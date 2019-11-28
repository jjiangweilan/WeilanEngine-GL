#pragma once

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <assimp/mesh.h>
#include <glad/glad.h>

namespace wlEngine
{
namespace Graphics
{
class Material;
struct Vertex
{
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;
    glm::vec3 bitangent;

/*Predefined Vertex*/
    static const std::vector<Vertex> quad;
    static const std::vector<unsigned int> quadElement;

    static const std::vector<Vertex> box;
    static const std::vector<unsigned int> boxElement;
};

enum PrimitiveMeshType
{
    Cube,
    Sphere
};

class Mesh
{
public:
    /**
 * @brief Construct a new Mesh object with a predefined mesh type
 * 
 * @param meshType 
 */
    Mesh(const PrimitiveMeshType &meshType, Material *mat, const std::string &name = "");
    Mesh(const std::vector<GLuint> &indices, const std::vector<Vertex> &vertices, Material *mat, const std::string &name = "");
    Mesh(std::vector<GLuint> &&indices, std::vector<Vertex> &&vertices, Material *mat, const std::string &name = "");
    Mesh(const Mesh &mesh) = default;
    Mesh(Mesh &&mesh) noexcept;
    Mesh(aiMesh *mesh, Material *m = nullptr, const std::string &name = "");
    Mesh() = default;

    Mesh &operator=(const Mesh &mesh) = default;
    Mesh &operator=(Mesh &&mesh);
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
    void setMaterial(const std::string &name) const;
	void setVertices(std::vector<Vertex>&& vertices, std::vector<GLuint>&& indices);

    const GLuint& GetVAO() const;
    const std::vector<GLuint>* GetIndices() const;
    const std::vector<Vertex>* GetVertices() const;
    const Material* GetMaterial() const {return m_material;}
    /**
 * @brief used in model to identify the material
 * 
 */
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
    friend class Model;
};
} // namespace Graphics
} // namespace wlEngine