#ifndef TEXTURE_H
#define TEXTURE_H

#include <string>
#include <SDL.h>
#include <glm/gtc/matrix_transform.hpp>

#include "Component.hpp"
#include "Component/Clip.hpp"

#include "GameObject/Entity.hpp"
#include "Graphics/Shader.hpp"
#include "Graphics/Mesh2D.hpp"
namespace WeilanEngine
{
/* class Texture
	 * @description: use initialized GraphicsManager to draw picture on
	 * windowRenderer
	 */
class Entity;
class Sprite : public Component
{
	COMPONENT_DECLARATION(Component, Sprite, 64);
	COMPONENT_EDITABLE_DEC();

public:
	Sprite(Entity *);
	Sprite(Entity *, const std::string &path);
	Sprite(Entity *, const std::string &path, const std::string &shader);
	Sprite(Entity *, void **args);
	~Sprite();

	void useShader(const std::string &);
	std::function<void()> beforeRenderFunc;
	std::function<void()> afterRenderFunc;
	void loadTexture(const std::string &path);
	void changeTexture(const int &loc, Graphics::Texture *texture);
	Graphics::Texture *getMainTexture() const;
	/**
		   * @brief the render system will use this to let user takes full control of the rendering
		   * sequence, instead of using the defualt render sequence
		   *
		   * @param view
		   * @param projection
		   *
		   */
	std::function<void(const glm::mat4 &, const glm::mat4 &)> draw;

	const Graphics::Shader *getShader() const;
	const Graphics::Mesh2D *getMesh() const;

	float transparency;

private:
	Graphics::Mesh2D m_mesh;
	Graphics::Shader *m_shader;
};
} // namespace WeilanEngine
#endif
