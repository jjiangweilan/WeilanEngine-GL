#pragma once
#include <vector>

#include "Mesh.hpp"
#include "../Graphics/Texture.hpp"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace wlEngine
{
class Model3D
{
public:
    Model3D() = default;
    Model3D(const std::string &path);
    const std::vector<Mesh> *getMeshes() const;

private:
    std::vector<Mesh> m_meshes;
    bool m_gammaCorrection;
    std::string m_directory;

    Model3D *loadModel(const std::string &name);
    void processNode(aiNode *node, const aiScene *scene);
    std::vector<Texture *> loadMaterialTextures(aiMaterial *mat, aiTextureType textureType, const TextureType& type, const aiScene *);

    friend class ResourceManager;
};
} // namespace wlEngine