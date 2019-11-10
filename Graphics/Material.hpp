#pragma once

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
     * seems as calling useShader and changeTextures
     * 
     * @param shader shader name
     * @param textures textures
     */
    Material(const std::string &shader, std::vector<Texture *> &&textures)
    {
        setShader(shader);
        changeTextures(std::move(textures));
    };
    Material(const std::string &shader, const std::vector<Texture *> &textures)
    {
        setShader(shader);
        m_textures = textures;
    }
    Material(const std::string &shader) : m_textures()
    {
        setShader(shader);
    }
    /**
     * @brief change the used shader
     * 
     * @param shader the name of the shader
     */
    void setShader(const std::string &shader);

    /**
     * @brief change the current textures
     * 
     * @param newTextures the textures to be moved
     */
    void changeTextures(std::vector<Texture *> &&newTextures)
    {
        m_textures = std::move(newTextures);
    }

    /**
     * @brief Get the Textures object
     * 
     * @return const std::vector<Texture*> 
     */
    const std::vector<Texture *> *getTextures() const
    {
        return &m_textures;
    }

    void useShader() const;
    const Shader *getShader() const;

private:
    std::vector<Texture *> m_textures;
    Shader *m_shader;
/* Static ----*/
public:
    static Material* get(const std::string& id);
    template<typename ...Args>
    static Material* add(const std::string& id, Args&& ... args);
    static void remove(const std::string& id);
private:
    static std::unordered_map<std::string, Material> collection;
};

template<typename ...Args>
Material* Material::add(const std::string& id, Args&& ... args)
{
	auto has = collection.find(id);
	if (has != collection.end()) return &has->second;

	auto pair = collection.emplace(std::piecewise_construct, std::forward_as_tuple(id), std::forward_as_tuple(args...));
	return &pair.first->second;
}
} // namespace Graphics
} // namespace wlEngine