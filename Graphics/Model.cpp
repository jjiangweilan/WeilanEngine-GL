#include "Model.hpp"

#include "../Manager/ResourceManager.hpp"
namespace wlEngine
{
namespace Graphics
{

Model::Model(std::vector<Mesh>&& meshes) {
    m_meshes = std::move(meshes);
}

Model::Model(const std::vector<Mesh>& meshes) {
    m_meshes = meshes;
}

Model::Model(const std::string &path) : m_id(path)
{
    loadModel(path);
}

Model::Model(Model&& model) {
	m_meshes = std::move(model.m_meshes);
	m_id = model.m_id;
	m_gammaCorrection = m_gammaCorrection;
}

Model::Model(const Model& model) {
	m_meshes = model.m_meshes;
	m_id = model.m_id;
	m_gammaCorrection = m_gammaCorrection;
}


Model *Model::loadModel(const std::string &path)
{
    Assimp::Importer importer;
    m_id = path;
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
    auto m_directory = m_id.substr(0, m_id.find_last_of('/'));
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
        auto mat = ResourceManager::get()->getMaterial(m_id + "-" + materialName);
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

Model *Model::get(const std::string &id)
{
    return &collection[id];
}

void Model::remove(const std::string &id)
{
    collection.erase(id);
}

std::unordered_map<std::string, Model> Model::collection = std::unordered_map<std::string, Model>();
} // namespace Graphics
} // namespace wlEngine