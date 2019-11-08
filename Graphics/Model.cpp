#include "Model.hpp"
#include "../Manager/ResourceManager.hpp"
namespace wlEngine
{
namespace Graphics
{

Model::Model(const std::string &path) : m_path(path)
{
    loadModel(path);
}

Model *Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    m_path = path;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return nullptr;
    }
    processNode(scene->mRootNode, scene);

    return this;
}

void Model::processNode(aiNode *node, const aiScene *scene)
{
    auto m_directory = m_path.substr(0, m_path.find_last_of('/'));
    for (size_t i = 0; i < node->mNumMeshes; i++)
    {
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        aiMaterial *material = scene->mMaterials[mesh->mMaterialIndex];

        //process textures from aiMaterial
        std::vector<Texture *> textures;
        aiString name;
        for (size_t aiType = aiTextureType_DIFFUSE; aiType < aiTextureType_UNKNOWN; aiType++)
        {
            for (size_t i = 0; i < material->GetTextureCount((aiTextureType)aiType); i++)
            {
                material->GetTexture((aiTextureType)aiType, i, &name);
                Texture *texture = ResourceManager::get()->getTexture(m_directory + "/" + name.C_Str(), (TextureType)aiType);
                textures.push_back(texture);
            }
        }

        std::string materialName = name.C_Str();
        auto ptr = materialName.find_first_of("_");
        if (ptr == materialName.npos)
            ptr = materialName.find_first_of("-");
        if (ptr == materialName.npos)
            ptr = materialName.find_first_of(" ");
        materialName = materialName.substr(0, ptr);
        auto mat = ResourceManager::get()->getMaterial(m_path + "-" + materialName);
        mat->setShader("Model");
        mat->changeTextures(std::move(textures));

        m_meshes.emplace_back(mesh, mat, materialName);
    }
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}
const std::vector<Mesh> *Model::getMeshes() const
{
    return &m_meshes;
}

const Mesh *Model::getMesh(const std::string &name) const
{
    for (auto &m : m_meshes)
    {
        if (m.name == name)
        {
            return &m;
        }
    }
    return nullptr;
}

void Model::addMesh(Mesh &&mesh)
{
    m_meshes.emplace_back(std::move(mesh));
}

void Model::addMesh(const Mesh &mesh)
{
    m_meshes.push_back(mesh);
}
} // namespace Graphics
} // namespace wlEngine