#include "Model3D.hpp"
#include "../Manager/ResourceManager.hpp"
namespace wlEngine
{
Model3D::Model3D(const std::string &path) : m_directory(path)
{
    loadModel(path);
}

Model3D* Model3D::loadModel(const std::string &path)
{
    Assimp::Importer importer;

    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    m_directory = path.substr(0, path.find_last_of('/'));
    processNode(scene->mRootNode, scene);

    return this;
}

void Model3D::processNode(aiNode *node, const aiScene *scene)
{
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];

        std::vector<Vertex> vertices;
        std::vector<GLuint> indices;
        std::vector<Texture*> textures;

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
                vertex.textureCoords = vec2;
            }
            else
            {
                vertex.textureCoords = glm::vec2(0.0f, 0.0f);
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

            vertices.push_back(vertex);
        }

        for (size_t i = 0; i < mesh->mNumFaces; i++)
        {
            aiFace face = mesh->mFaces[i];
            for (size_t j = 0; j < face.mNumIndices; j++)
            {
                indices.push_back(face.mIndices[j]);
            }
        }

        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        std::vector<Texture*> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture*> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture*> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture*> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        m_meshes.emplace_back(std::move(textures), std::move(indices), std::move(vertices));
    }

    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}

std::vector<Texture*> Model3D::loadMaterialTextures(aiMaterial *mat, aiTextureType type, std::string typeName, const aiScene *scene)
{
    std::vector<Texture*> textures;

    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);
        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        Texture* texture = ResourceManager::get()->getTexture(m_directory + "/" + str.C_Str());
        bool skip = false;
        textures.push_back(texture);
    }
    return textures;
}

const std::vector<Mesh> *Model3D::getMeshes() const 
{
    return &m_meshes;
}
} // namespace wlEngine