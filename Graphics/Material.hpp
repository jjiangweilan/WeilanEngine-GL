#pragma once

#include <vector>
#include <glad/glad.h>
#include <assimp/material.h>

namespace wlEngine 
{
class Texture;
class Shader;
class RenderSystem;
class Material
{
public:
    Material();
    /**
     * @brief Construct a new Material object
     * seems as calling useShader and changeTextures
     * 
     * @param shader shader name
     * @param textures textures
     */
    Material(const std::string& shader, std::vector<Texture*>&& textures);
    /**
     * @brief change the used shader
     * 
     * @param shader the name of the shader
     */
    void setShader(const std::string& shader);

    /**
     * @brief change the current textures
     * 
     * @param newTextures the textures to be moved
     */
    void changeTextures(std::vector<Texture*>&& newTextures)
    {
        m_textures = std::move(newTextures);
    }

    /**
     * @brief Get the Textures object
     * 
     * @return const std::vector<Texture*> 
     */
    const std::vector<Texture*>* getTextures() const
    {
        return &m_textures;
    }

    void useShader()const;
private:
    std::vector<Texture*> m_textures;
    Shader* m_shader;
};
}