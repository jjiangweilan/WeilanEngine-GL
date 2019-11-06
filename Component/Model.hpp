#pragma once
#include "Component.hpp"
#include "../Graphics/Mesh.hpp"

#include <vector>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <stb_image.hpp>
#include <functional>
namespace wlEngine
{
class Model : public Component
{
    COMPONENT_DECLARATION(Component, Model, 50);

public:
    std::vector<Mesh> meshes;
    bool gammaCorrection;
    std::string directory;


    Model(Entity *go, const std::string &path, bool gamma = false) : Component(go), gammaCorrection(gamma)
    {
        loadModel(path);
        shader = Shader::collection["Model"];
    }

    void destruct(Entity *go) override;

    void useShader(const std::string &name)
    {
        shader = Shader::collection.find(name)->second;
    }

    Shader *shader;
    std::function<void()> beforeRenderFunc;
    std::function<void()> afterRenderFunc;

private:
    void loadModel(const std::string &name);
    void processNode(aiNode *node, const aiScene *scene);
    std::vector<Texture*> loadMaterialTextures(aiMaterial *mat, aiTextureType textureType, std::string typeName, const aiScene *);
    size_t TextureFromFile(const char *path, const std::string &directory, bool gamma);
};
} // namespace wlEngine
