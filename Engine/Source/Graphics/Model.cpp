#include "Graphics/Model.hpp"

#include "Manager/ResourceManager.hpp"

#include <cfloat>
namespace wlEngine
{
namespace Graphics
{

Model::Model(std::vector<Mesh> &&meshes)
{
    m_meshes = std::move(meshes);
}

Model::Model(const std::vector<Mesh> &meshes)
{
    m_meshes = meshes;
}

Model::Model(const std::string &path) : m_id(path)
{
    loadModel(path);
}

Model::Model(Model &&model)
{
    m_meshes = std::move(model.m_meshes);
    m_id = model.m_id;
    m_gammaCorrection = m_gammaCorrection;
}

Model::Model(const Model &model)
{
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
            int maxTextureCount = material->GetTextureCount((aiTextureType)aiType);
            for (size_t i = 0; i < maxTextureCount; i++)
            {
                material->GetTexture((aiTextureType)aiType, i, &name);
                auto path = m_directory + "/" + name.C_Str();
                Texture *texture = Texture::Add(path, path, (TextureType)aiType);
                textures.push_back(texture);
            }
        }

		std::string matName = name.C_Str();
		auto pos = matName.find_last_of("_");
		if (pos == matName.npos)
			pos = matName.find_last_of("-");
		if (pos == matName.npos)
			pos = matName.find_last_of(" ");
		if (pos == matName.npos) pos = matName.size();
		matName = matName.substr(0, pos);

		auto mat = Material::Add(m_id + "-" + matName , "Model", textures);
        mat->name = m_id + "-" + matName;

        m_meshes.emplace_back(mesh, mat, std::to_string(mesh->mMaterialIndex));
    }
    for (size_t i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }
}
const std::vector<Mesh> *Model::GetMeshes() const
{
    return &m_meshes;
}

const Mesh *Model::GetMesh(const std::string &name) const
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

void Model::AddMesh(Mesh &&mesh)
{
    m_meshes.emplace_back(std::move(mesh));
}

void Model::AddMesh(const Mesh &mesh)
{
    m_meshes.push_back(mesh);
}

Model *Model::Get(const std::string &id)
{
    auto iter = collection.find(id);
    if (iter == collection.end())
        return nullptr;
    return &iter->second;
}

void Model::Remove(const std::string &id)
{
    collection.erase(id);
}

AABB Model::GetAABB() const
{
    float top = FLT_MIN;
    float down = FLT_MAX;
    float front = FLT_MIN;
    float back = FLT_MAX;
    float left = FLT_MAX;
    float right = FLT_MIN;

    for (auto &m : m_meshes)
    {
        for (auto &vert : m.m_vertices)
        {
            auto &pos = vert.position;
            if (pos.x > right)
                right = pos.x;
            if (pos.x < left)
                left = pos.x;
            if (pos.y > top)
                top = pos.y;
            if (pos.y < down)
                down = pos.y;
            if (pos.z > front)
                front = pos.z;
            if (pos.z < back)
                back = pos.z;
        }
    }
    return {{left, down, back}, {right, top, front}};
}
std::unordered_map<std::string, Model> Model::collection = std::unordered_map<std::string, Model>();
} // namespace Graphics
} // namespace wlEngine