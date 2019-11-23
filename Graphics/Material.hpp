#pragma once
#include "ShaderParameter.hpp"

#include <vector>
#include <glad/glad.h>
#include <string>
#include <unordered_map>

namespace wlEngine
{
class RenderSystem;
namespace Graphics
{
class Texture;
class Shader;
class Material
{
public:
    Material();
    /**
     * @brief Construct a new Material object
     * seems as calling UseShader and ChangeTextures
     * 
     * @param shader shader name
     * @param textures textures
     */
    Material(const std::string &shader, std::vector<Texture *> &&textures)
    {
        SetShader(shader);
        m_parameters.SetTextures(textures);
    };

    Material(const std::string &shader, const std::vector<Texture *> &textures)
    {
        SetShader(shader);
		m_parameters.SetTextures(textures);
    }

    Material(Shader* shader) :  m_shader(shader)
    {
    }

    Material(const std::string &shader) 
    {
        SetShader(shader);
    }

    Material(const Material& other);
    Material(Material&& other);
    /**
     * @brief change the used shader
     * 
     * @param shader the name of the shader
     */
    void SetShader(const std::string &shader);


    /**
 * @brief return a copy of this material
 * 
 * @return Material 
 */
    Material Clone() const;

    void useShader() const;
    const Shader *getShader() const;
    ShaderParameter *GetParameters() const;

protected:
    mutable ShaderParameter m_parameters;
    Shader *m_shader;
    /* Static ----*/
public:
    static Material *get(const std::string &id);
    template <typename... Args>
    static Material *add(const std::string &id, Args &&... args);
    static void remove(const std::string &id);

private:
    static std::unordered_map<std::string, Material> collection;
};

template <typename... Args>
Material *Material::add(const std::string &id, Args &&... args)
{
    auto has = collection.find(id);
    if (has != collection.end())
        return &has->second;
    auto pair = collection.emplace(id, Material(args...));
    return &pair.first->second;
}
} // namespace Graphics
} // namespace wlEngine