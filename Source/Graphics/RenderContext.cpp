#include "Graphics/RenderContext.hpp"
#include "System/RenderSystem.hpp"

namespace WeilanEngine
{
namespace Graphics
{
RenderContext::FramebufferAttachment::FramebufferAttachment() : colors(), depth(), stencil() {}
RenderContext::FramebufferAttachment::~FramebufferAttachment()
{
}
std::vector<GLenum> RenderContext::FramebufferAttachment::GetColorAttachmentArray() const
{
	std::vector<GLenum> rlt(colors.size());
	for (size_t i = 0; i < colors.size(); i++)
	{
		rlt[i] = GL_COLOR_ATTACHMENT0 + i;
	}
	return rlt;
}

RenderContext::RenderContext() : m_attachment(), m_framebuffer(0), m_drawFlag(false)
{
}

RenderContext::~RenderContext()
{
	glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderContext::GenFramebuffer()
{
	if (m_framebuffer != 0)
		glDeleteFramebuffers(1, &m_framebuffer);
	m_attachment.colors.clear();
	if (m_attachment.depth)
		m_attachment.depth->Free();
	if (m_attachment.stencil)
		m_attachment.stencil->Free();
	glGenFramebuffers(1, &m_framebuffer);
}

void RenderContext::AttachTexture2D(const AttachmentType &attachmentType,
									const Graphics::Texture::InternalFormat &internalFormat,
									const Graphics::Texture::DataFormat &format,
									const Graphics::Texture::DataType &type,
									const unsigned int &width,
									const unsigned int &height)
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	auto texture = std::make_unique<Graphics::Texture>(width, height, internalFormat, format, type);
	if (attachmentType == AttachmentType::Color)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   attachmentType + m_attachment.colors.size(),
							   GL_TEXTURE_2D,
							   texture->GetId(),
							   0);
		m_attachment.colors.emplace_back(std::move(texture));
		glDrawBuffers(m_attachment.colors.size(), m_attachment.GetColorAttachmentArray().data());
	}
	else if (attachmentType == AttachmentType::Depth)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   attachmentType,
							   GL_TEXTURE_2D,
							   texture->GetId(),
							   0);
		m_attachment.depth = std::move(texture);
	}
	else if (attachmentType == AttachmentType::Stencil)
	{
		glFramebufferTexture2D(GL_FRAMEBUFFER,
							   attachmentType,
							   GL_TEXTURE_2D,
							   texture->GetId(),
							   0);
		m_attachment.stencil = std::move(texture);
	}
	auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE)
	{
		std::cerr << "frame buffer not completed! " << status << std::endl;
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderContext::Use() const
{
	glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
	glClearColor(1.0, 1.0, 1.0, 1.0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
}

const GLuint &RenderContext::GetFramebuffer() const
{
	return m_framebuffer;
}
const RenderContext::FramebufferAttachment *RenderContext::GetAttachment() const
{
	return &m_attachment;
}

void RenderContext::AttachTexture2D(const PredefinedAttachmentType &choice)
{
	switch (choice)
	{
	case PredefinedAttachmentType::Standard:
		AttachTexture2D(RenderContext::AttachmentType::Color,
						Graphics::Texture::InternalFormat::RGBA8,
						Graphics::Texture::DataFormat::RGBA,
						Graphics::Texture::DataType::UnsignedByte,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().x,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().y);
		break;
	case PredefinedAttachmentType::HDR:

		AttachTexture2D(RenderContext::AttachmentType::Color,
						Graphics::Texture::InternalFormat::RGBA16F,
						Graphics::Texture::DataFormat::RGBA,
						Graphics::Texture::DataType::HalfFloat,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().x,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().y);
		break;

	case PredefinedAttachmentType::Depth:
		AttachTexture2D(RenderContext::Depth,
						Graphics::Texture::InternalFormat::DepthComponent,
						Graphics::Texture::DataFormat::Depth,
						Graphics::Texture::DataType::UnsignedInt,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().x,
						WeilanEngine::RenderSystem::Get()->GetSceneSize().y);
		break;
	}
}
void RenderContext::SetDrawFlag(const bool &draw)
{
	m_drawFlag = draw;
}
const bool &RenderContext::GetDrawFlag() const
{
	return m_drawFlag;
}
const Graphics::Texture *RenderContext::GetColorAttachment(const size_t &loc) const
{
	return m_attachment.colors[loc].get();
}
const Graphics::Texture *RenderContext::GetDepthAttachment() const
{
	return m_attachment.depth.get();
}
const Graphics::Texture *RenderContext::GetStencilAttachment() const
{
	return m_attachment.stencil.get();
}
} // namespace Graphics
} // namespace WeilanEngine