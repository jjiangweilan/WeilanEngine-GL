#include "Model.hpp"

namespace wlEngine {
    COMPONENT_DEFINATION(Component, Model, 50);

    void Model::loadModel(const std::string& path) {
        Assimp::Importer importer;

        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);

        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

    void Model::processNode(aiNode* node, const aiScene* scene) {
        for (size_t i = 0; i < node->mNumMeshes; i++) {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];

            
            std::vector<Vertex> vertices;
            std::vector<GLuint> indices;
            std::vector<Texture3D> textures;

            for (size_t i = 0; i < mesh->mNumVertices; i++) {
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

                if (mesh->mTextureCoords[0]) {
                    glm::vec2 vec2;
                    vec2.x = mesh->mTextureCoords[0][i].x;
                    vec2.y = mesh->mTextureCoords[0][i].y;
                    vertex.textureCoords = vec2;
                }
                else {
                    vertex.textureCoords = glm::vec2(0.0f, 0.0f);
                }

                if (mesh->mTangents) {
                    // tangent
                    vec3.x = mesh->mTangents[i].x;
                    vec3.y = mesh->mTangents[i].y;
                    vec3.z = mesh->mTangents[i].z;
                    vertex.tangent = vec3;
                }

                if (mesh->mBitangents) {
                    // bitangent
                    vec3.x = mesh->mBitangents[i].x;
                    vec3.y = mesh->mBitangents[i].y;
                    vec3.z = mesh->mBitangents[i].z;
                    vertex.bitangent = vec3;
                }

                vertices.push_back(vertex);
            }

            for (size_t i = 0; i < mesh->mNumFaces; i++) {
                aiFace face = mesh->mFaces[i];
                for (size_t j = 0; j < face.mNumIndices; j++) {
                    indices.push_back(face.mIndices[j]);
                }
            }

            aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

            std::vector<Texture3D> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse", scene);
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture3D> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular", scene);
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture3D> normalMaps = loadMaterialTextures(material, aiTextureType_HEIGHT, "texture_normal", scene);
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture3D> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_height", scene);
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

            meshes.emplace_back(std::move(textures), std::move(indices), std::move(vertices));
        }

        for (size_t i = 0; i < node->mNumChildren; i++) {
            processNode(node->mChildren[i], scene);
        }
    }

    std::vector<Texture3D> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName, const aiScene* scene) {
        std::vector<Texture3D> textures;

        for(unsigned int i = 0; i < mat->GetTextureCount(type); i++)
        {
            aiString str;
            mat->GetTexture(type, i, &str);
            // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
            Texture3D texture;
            bool skip = false;
            for (auto& i : loaded) {
                if (i.resourcePath.compare(str.C_Str()) == 0) {
                    texture.id = i.id;
                    texture.type = i.type;
                    texture.resourcePath = i.resourcePath;
                    skip = true;
                    break;
                }
            }
            if (!skip) {
                texture.resourcePath = str.C_Str();
                texture.id = TextureFromFile(str.C_Str(), this->directory, gammaCorrection);
                texture.type = typeName;
                loaded.push_back(texture);
            }

            textures.push_back(texture);
        }
        return textures;
    }

    size_t Model::TextureFromFile(const char *path, const std::string &directory, bool gamma) {
        std::string filename = std::string(path);
        filename = directory + '/' + filename;

        unsigned int textureID;
        glGenTextures(1, &textureID);

        int width, height, nrComponents;
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);
        if(data){
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
        }
        else
        {
            std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }

    void Model::destruct(Entity* go) {
        if (entities) entities->erase(go);
    }
}
