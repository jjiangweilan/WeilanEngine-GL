#pragma once

#include "Texture.hpp"
#include "Mesh.hpp"
namespace WeilanEngine
{
namespace Graphics
{
class Shader;
class Model;
class Texture;
enum class PredefinedAttachmentType
{
	HDR,
	Standard,
	Depth
};
class RenderContext
{
public:
	enum AttachmentType
	{
		Color = GL_COLOR_ATTACHMENT0,
		Stencil = GL_STENCIL_ATTACHMENT,
		Depth = GL_DEPTH_ATTACHMENT,
	};
	struct FramebufferAttachment
	{
		std::vector<std::unique_ptr<Graphics::Texture>> colors;
		std::unique_ptr<Graphics::Texture> depth;
		std::unique_ptr<Graphics::Texture> stencil;
		std::vector<GLenum> GetColorAttachmentArray() const;
		FramebufferAttachment();
		~FramebufferAttachment();
	};

	RenderContext();
	~RenderContext();

	void GenFramebuffer();
	void AttachTexture2D(const AttachmentType &attachmentType,
						 const Graphics::Texture::InternalFormat &internalFormat,
						 const Graphics::Texture::DataFormat &format,
						 const Graphics::Texture::DataType &type,
						 const unsigned int &width,
						 const unsigned int &height);
	void AttachTexture2D(const PredefinedAttachmentType &);

	void Use() const;
	/**
			 * @brief Set the Render Loop object
			 *
			 * @param start
			 * @param drawMesh the mesh used to draw the texture from start
			 * @param loop
			 */
	void SetRenderLoop(RenderContext *start, const std::vector<std::pair<std::string, const Graphics::Texture *>> &textures, const Graphics::Mesh &drawMesh, const size_t &loop);

	const GLuint &GetFramebuffer() const;
	const FramebufferAttachment *GetAttachment() const;
	const Graphics::Texture *GetColorAttachment(const size_t &loc) const;
	const Graphics::Texture *GetDepthAttachment() const;
	const Graphics::Texture *GetStencilAttachment() const;

	void SetDrawFlag(const bool &draw);
	const bool &GetDrawFlag() const;

private:
	GLuint m_framebuffer;
	FramebufferAttachment m_attachment;

	bool m_drawFlag;

	friend class RenderSystem;
};
} // namespace Graphics
} // namespace WeilanEngine