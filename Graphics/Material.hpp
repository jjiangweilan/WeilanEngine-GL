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

    Material(const std::string &shader, const std::vector<std::pair<std::string, Texture *>> &textures)
    {
        SetShader(shader);
        for (int i = 0; i < textures.size(); i++)
        {
            m_parameters.SetParameter(textures[i].first,
                                      TextureUnitBinding(i, textures[i].second));
        }
    }

    Material(Shader *shader) : m_shader(shader)
    {
    }

    Material(const std::string &shader)
    {
        SetShader(shader);
    }

    Material(const Material &other);
    Material(Material &&other);
    /**
     * @brief change the used shader
     * 
     * @param shader the name of the shader
     */
    void SetShader(const std::string &shader);

    Material& operator=(const Material&);
    Material& operator=(Material&&);

    /**
 * @brief return a copy of this material
 * 
 * @return Material 
 */
    Material Clone() const;

    void UseMaterial() const;
    const Shader *GetShader() const;
    ShaderParameter *GetParameters() const;
    void ReloadShader() const;
    
    std::string name;
protected:
    mutable ShaderParameter m_parameters;
    Shader *m_shader;
    /* Static ----*/
public:
    static Material *Get(const std::string &id);
    template <typename... Args>
    static Material *Add(const std::string &id, Args &&... args);
    static void remove(const std::string &id);

private:
    static std::unordered_map<std::string, Material> collection;
};

template <typename... Args>
Material *Material::Add(const std::string &id, Args &&... args)
{
    auto has = collection.find(id);
    if (has != collection.end())
        return &has->second;
    auto pair = collection.emplace(id, Material(args...));
    pair.first->second.name = id;
    return &pair.first->second;
}
} // namespace Graphics
} // namespace wlEngine