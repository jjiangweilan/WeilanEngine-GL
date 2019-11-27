#include "Mesh.hpp"
#include "../Manager/ResourceManager.hpp"
namespace wlEngine
{
namespace Graphics
{
const std::vector<Vertex> Vertex::quad = {
    {glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
};

const std::vector<unsigned int> Vertex::quadElement = {
    0,1,3,
    1,2,3
};

Mesh::Mesh(const PrimitiveMeshType &meshType, Material *mat, const std::string &name) : m_material(mat), name(name)
{
    switch (meshType)
    {
    case Cube:

        break;
    case Sphere:
        break;
    }
}
Mesh::Mesh(const std::vector<GLuint> &indices, const std::vector<Vertex> &vertices, Material *mat, const std::string &name) : m_indices(indices), m_vertices(vertices), m_material(mat), name(name)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    setupMesh();
}
Mesh::Mesh(std::vector<GLuint> &&indices, std::vector<Vertex> &&vertices, Material *mat, const std::string &name) : m_indices(indices), m_vertices(vertices), m_material(mat), name(name)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    setupMesh();
}
void Mesh::setupMesh()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(GLuint), &m_indices[0], GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, texCoord));

    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, normal));

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, tangent));

    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void *)offsetof(Vertex, bitangent));

    glBindVertexArray(0);
}

Mesh::Mesh(Mesh &&mesh) noexcept
{
    this->m_vertices = std::move(mesh.m_vertices);
    this->m_indices = std::move(mesh.m_indices);
    this->EBO = mesh.EBO;
    this->VAO = mesh.VAO;
    this->VBO = mesh.VBO;
    this->m_material = mesh.m_material;
    this->name = mesh.name;
}
Mesh &Mesh::operator=(Mesh &&mesh)
{
    this->m_vertices = std::move(mesh.m_vertices);
    this->m_indices = std::move(mesh.m_indices);
    this->EBO = mesh.EBO;
    this->VAO = mesh.VAO;
    this->VBO = mesh.VBO;
    this->m_material = mesh.m_material;
    this->name = mesh.name;
    return *this;
}
Mesh::Mesh(aiMesh *mesh, Material *m, const std::string &name) : m_material(m), name(name)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    loadFromAssimp(mesh);
    setupMesh();
}
void Mesh::loadFromAssimp(aiMesh *mesh)
{
    for (size_t i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vec3;
        vec3.x = mesh->mVertices[i].x;
        vec3.y = mesh->mVertices[i].y;
        vec3.z = mesh->mVertices[i].z;
        vertex.position = vec3;

        vec3.x = mesh->mNormals[i].x;
        vec3.y = mesh->mNormals[i].y;
        vec3.z = mesh->mNormals[i].z;
        vertex.normal = vec3;

        if (mesh->mTextureCoords[0])
        {
            glm::vec2 vec2;
            vec2.x = mesh->mTextureCoords[0][i].x;
            vec2.y = mesh->mTextureCoords[0][i].y;
            vertex.texCoord = vec2;
        }
        else
        {
            vertex.texCoord = glm::vec2(0.0f, 0.0f);
        }

        if (mesh->mTangents)
        {
            // tangent
            vec3.x = mesh->mTangents[i].x;
            vec3.y = mesh->mTangents[i].y;
            vec3.z = mesh->mTangents[i].z;
            vertex.tangent = vec3;
        }

        if (mesh->mBitangents)
        {
            // bitangent
            vec3.x = mesh->mBitangents[i].x;
            vec3.y = mesh->mBitangents[i].y;
            vec3.z = mesh->mBitangents[i].z;
            vertex.bitangent = vec3;
        }

        m_vertices.push_back(vertex);
    }

    for (size_t i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        for (size_t j = 0; j < face.mNumIndices; j++)
        {
            m_indices.push_back(face.mIndices[j]);
        }
    }
}
void Mesh::setMaterial(Material *material) const
{
    m_material = material;
}
void Mesh::setMaterial(const std::string &name) const
{
    m_material = Material::get(name);
}

void Mesh::setVertices(std::vector<Vertex> &&vertices, std::vector<GLuint> &&indices)
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    m_indices = std::move(indices);
    m_vertices = std::move(vertices);
    setupMesh();
}
const GLuint &Mesh::GetVAO() const
{
    return VAO;
}
const std::vector<GLuint> *Mesh::GetIndices() const
{
    return &m_indices;
}
const std::vector<Vertex> *Mesh::GetVertices() const
{
    return &m_vertices;
}
} // namespace Graphics
} // namespace wlEngine