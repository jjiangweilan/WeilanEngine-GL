#include "Mesh.hpp"
#include "../Manager/ResourceManager.hpp"
namespace wlEngine
{
namespace Graphics
{
Mesh::Mesh(const PrimitiveMeshType &meshType, Material *mat, const std::string &name) : name(name), m_materialIndex(0)
{
    assert(0 && "not yet implemented");
    SetMaterialWithoutDetach(mat);
    switch (meshType)
    {
    case Cube:

        break;
    case Sphere:
        break;
    }
}
Mesh::Mesh(const std::vector<GLuint> &indices, const std::vector<Vertex> &vertices, Material *mat, const std::string &name) : m_indices(indices), m_vertices(vertices), name(name), m_materialIndex(0)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    SetMaterialWithoutDetach(mat);
    setupMesh();
}
Mesh::Mesh(std::vector<GLuint> &&indices, std::vector<Vertex> &&vertices, Material *mat, const std::string &name) : m_indices(indices), m_vertices(vertices),  name(name), m_materialIndex(0)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    SetMaterialWithoutDetach(mat);
    setupMesh();
}
Mesh::Mesh(aiMesh *mesh, Material *m, const std::string &name) : m_material(m), name(name), m_materialIndex(0)
{
    EBO = 0;
    VAO = 0;
    VBO = 0;
    SetMaterialWithoutDetach(m);
    loadFromAssimp(mesh);
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
    this->m_uniqueMaterial = std::move(m_uniqueMaterial);
    this->name = mesh.name;
    this->m_materialIndex = mesh.m_materialIndex;
}
Mesh &Mesh::operator=(Mesh &&mesh)
{
    this->m_vertices = std::move(mesh.m_vertices);
    this->m_indices = std::move(mesh.m_indices);
    this->EBO = mesh.EBO;
    this->VAO = mesh.VAO;
    this->VBO = mesh.VBO;
    this->m_material = mesh.m_material;
    this->m_uniqueMaterial = std::move(m_uniqueMaterial);
    this->name = mesh.name;
    this->m_materialIndex = mesh.m_materialIndex;
    return *this;
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
void Mesh::SetMaterial(Material *material) const
{
    material->DetachFromMesh(this);
    SetMaterialWithoutDetach(material);
    std::cout << "wawa";
}

void Mesh::SetMaterialWithoutDetach(Material* material) const
{
    m_material = material;
    m_materialIndex = material->AttachToMesh(this);
}

Mesh &Mesh::operator=(const Mesh &other)
{
    name = other.name;
    m_indices = other.m_indices;
    m_vertices = other.m_vertices;

    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;

    m_material = nullptr;
    if (other.m_material)
    {
        m_uniqueMaterial = std::make_unique<Material>(*other.m_material);
    }
    else
    {
        m_uniqueMaterial = std::make_unique<Material>(*other.m_uniqueMaterial);
    }
    m_uniqueMaterial->AttachToMesh(this);
    return *this;
}

Mesh::Mesh(const Mesh &other)
{
    name = other.name;
    m_indices = other.m_indices;
    m_vertices = other.m_vertices;

    VAO = other.VAO;
    VBO = other.VBO;
    EBO = other.EBO;

    m_material = nullptr;
    if (other.m_material)
    {
        m_uniqueMaterial = std::make_unique<Material>(*other.m_material);
    }
    else
    {
        m_uniqueMaterial = std::make_unique<Material>(*other.m_uniqueMaterial);
    }
    m_uniqueMaterial->AttachToMesh(this);
}

void Mesh::SetMaterial(const std::string &name) const
{
    SetMaterial(Material::Get(name));
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

const std::vector<Vertex> Vertex::quad = {
    {glm::vec3(1.0, 1.0, 0.0), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(1.0, -1.0, 0.0), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(-1.0, -1.0, 0.0), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
    {glm::vec3(-1.0, 1.0, 0.0), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 0.0, 0.0)},
};

const std::vector<unsigned int> Vertex::quadElement = {
    0, 1, 3,
    1, 2, 3};
#define oneThird (1 / 3.0)
const std::vector<Vertex> Vertex::boxQuad = {
    {
        {glm::vec3(0.5, 0.5, 0.5), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, 0.5, 0.5), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, 0.5, -0.5), glm::vec2(0.25, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, 0.5, -0.5), glm::vec2(0.5, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

        {glm::vec3(0.5, 0.5, 0.5), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, -0.5, 0.5), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, 0.5, 0.5), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

        {glm::vec3(-0.5, 0.5, 0.5), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, 0.5, -0.5), glm::vec2(0.0, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

        {glm::vec3(0.5, -0.5, 0.5), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0.25, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, -0.5, -0.5), glm::vec2(0.5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

        {glm::vec3(0.5, 0.5, 0.5), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, -0.5, 0.5), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, -0.5, -0.5), glm::vec2(0.75, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, 0.5, -0.5), glm::vec2(0.75, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

        {glm::vec3(0.5, 0.5, -0.5), glm::vec2(0.75, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(0.5, -0.5, -0.5), glm::vec2(0.75, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, -0.5, -0.5), glm::vec2(1, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
        {glm::vec3(-0.5, 0.5, -0.5), glm::vec2(1, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    }};

const std::vector<Vertex> Vertex::box = {
    {glm::vec3(0.5, 0.5, 0.5), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(-0.5, 0.5, 0.5), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(-0.5, 0.5, -0.5), glm::vec2(0.25, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(0.5, 0.5, -0.5), glm::vec2(0.5, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(0.5, -0.5, 0.5), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(-0.5, -0.5, 0.5), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(-0.5, -0.5, -0.5), glm::vec2(0.25, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    {glm::vec3(0.5, -0.5, -0.5), glm::vec2(0.5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
};

const std::vector<unsigned int> Vertex::boxElementQuad = {
    0, 1, 2, 3,
    4, 5, 6, 7,
    8, 9, 10, 11,
    12, 13, 14, 15,
    16, 17, 18, 19,
    20, 21, 22, 23};

const std::vector<unsigned int> Vertex::boxElement = {
    0, 1, 2, 0, 2, 3,
    0, 4, 1, 1, 4, 5,
    4, 5, 6, 4, 6, 7,
    3, 7, 2, 7, 6, 2,
    2, 1, 0, 0, 1, 5,
    0, 3, 7, 0, 7, 4};
} // namespace Graphics
} // namespace wlEngine