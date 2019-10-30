#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "Struct/Clip.hpp"

#include "../GameObject/Entity.hpp"
#include "../Graphics/Shader.hpp"
#include "../Graphics/Texture.hpp"
namespace wlEngine
{
/* class Texture
 * @description: use initialized GraphicsManager to draw picture on
 * windowRenderer
 */
class Entity;
class Sprite : public Component
{
  COMPONENT_DECLARATION(Component, Sprite, 100);
  COMPONENT_EDITABLE_DEC();

public:
  Sprite(Entity *);
  Sprite(Entity *, const std::string &path);
  Sprite(Entity *, const std::string &path, const std::string &shader);
  Sprite(Entity *, void **args);
  ~Sprite();

  void useShader(const std::string &);
  void addTexture(const std::string &name, const std::string &path);
  void removeTexture(const std::string &name);
  std::function<void()> beforeRenderFunc;
  std::function<void()> afterRenderFunc;
  /**
     * @brief the render system will use this to let user takes full control of the rendering
     * sequence, instead of using the defualt render sequence
     * 
     * @param view
     * @param projection
     * 
     */
  std::function<void(const glm::mat4 &, const glm::mat4 &)> draw;

  void setMainTexture(Texture* t);

  const std::map<std::string, Texture*> getTextures() const;
  const Texture* getMainTexture() const;
  const Shader* getShader() const;

  float transparency;

private:
  std::map<std::string, Texture *> m_textures;
  Texture *m_mainTexture;
  Shader *m_shader;
};
} // namespace wlEngine
#endif
