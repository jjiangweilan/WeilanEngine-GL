#pragma once
#include <vector>

#include "Mesh.hpp"
#include "../Graphics/Texture.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace wlEngine
{
class ResourceManager;
namespace Graphics
{
class Model
{
public:
    Model() = default;
    Model(const std::string &path);
    const std::vector<Mesh> *getMeshes() const;
    /**
     * @brief Get the Mesh object
     * 
     * @param name the name of the mesh
     * @return const Mesh* 
     */
    const Mesh* getMesh(const std::string& name) const;

private:
    std::vector<Mesh> m_meshes;
    bool m_gammaCorrection;
    std::string m_path;

    Model *loadModel(const std::string &name);
    void processNode(aiNode *node, const aiScene *scene);
    std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType textureType, const TextureType &type, const aiScene *);

    friend class wlEngine::ResourceManager;
};
} // namespace Graphics
} // namespace wlEngine