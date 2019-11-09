#pragma once

#include "Mesh.hpp"
#include "../Graphics/Texture.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <vector>
#include <unordered_map>

namespace wlEngine
{
namespace Graphics
{
class Model
{
public:
    Model() = default;
    Model(const std::string &path);
    Model(std::vector<Mesh>&& meshes);
	Model(const std::vector<Mesh>& meshes);
	Model(Model&& model);
	Model(const Model& model);

    
    const std::vector<Mesh> *getMeshes() const;
    /**
     * @brief Get the Mesh object
     * 
     * @param name the name of the mesh
     * @return const Mesh* 
     */
    const Mesh* getMesh(const std::string& name) const;
    /**
     * @brief move a mesh to the meshes
     * 
     * @param mesh 
     */
	void addMesh(Mesh&& mesh);
    /**
     * @brief copy a mesh to the meshes
     * 
     * @param mesh 
     */
	void addMesh(const Mesh& mesh);

private:
    std::vector<Mesh> m_meshes;
    bool m_gammaCorrection;
    std::string m_id;

    Model *loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType textureType, const TextureType &type, const aiScene *);

/* Static ----*/
public:
    static Model* get(const std::string& id);
    template<typename ...Args>
    static Model* add(const std::string& id, Args&& ... args);
    static void remove(const std::string& id);
private:
    static std::unordered_map<std::string, Model> collection;
};

template<typename ...Args>
Model* Model::add(const std::string& id, Args&& ... args)
{
    auto pair = collection.emplace(std::make_pair(id, Model(std::forward<Args>(args)...)));
    if (pair.second == false) return nullptr;
	return &pair.first->second;
}

} // namespace Graphics
} // namespace wlEngine