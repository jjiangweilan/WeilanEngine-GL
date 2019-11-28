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
class GameEditor;
namespace Graphics
{
struct AABB {
    glm::vec3 min;
    glm::vec3 max;
};
class Model
{
public:
    Model() = default;
    Model(const std::string &path);
    Model(std::vector<Mesh>&& meshes);
	Model(const std::vector<Mesh>& meshes);
	Model(Model&& model);
	Model(const Model& model);

    
    const std::vector<Mesh> *GetMeshes() const;
    /**
     * @brief Get the Mesh object
     * 
     * @param name the name of the mesh
     * @return const Mesh* 
     */
    const Mesh* GetMesh(const std::string& name) const;
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

    AABB getAABB() const;
private:
    std::vector<Mesh> m_meshes;
    bool m_gammaCorrection;
    std::string m_id;

    Model *loadModel(const std::string &path);
    void processNode(aiNode *node, const aiScene *scene);
    //std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType textureType, const TextureType &type, const aiScene *);

/* Static ----*/
public:
    static Model* get(const std::string& id);
    template<typename ...Args>
    static Model* Add(const std::string& id, Args&& ... args);
    static void remove(const std::string& id);
private:
    static std::unordered_map<std::string, Model> collection;

    friend class GameEditor;
};

template<typename ...Args>
Model* Model::Add(const std::string& id, Args&& ... args)
{
	auto has = collection.find(id);
	if (has != collection.end()) return &has->second;
//std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...)
	auto pair = collection.emplace(id, args...);
	return &pair.first->second;
}

} // namespace Graphics
} // namespace wlEngine