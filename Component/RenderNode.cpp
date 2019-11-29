#include "RenderNode.hpp"
#include "../Graphics/Material.hpp"
#include "../System/RenderSystem.hpp"

namespace wlEngine
{
COMPONENT_DEFINATION_NEW(Component, RenderNode)

RenderNode::InputSource::InputSource(const std::vector<RenderNode *>&nodes, const Graphics::Mesh &mesh) : nodes(nodes), mesh(mesh) {}
RenderNode::FramebufferAttachment::FramebufferAttachment() : colors(), depth(), stencil() {}
RenderNode::FramebufferAttachment::~FramebufferAttachment()
{
}
std::vector<GLenum> RenderNode::FramebufferAttachment::GetColorAttachmentArray() const
{
    std::vector<GLenum> rlt(colors.size());
    for (size_t i = 0; i < colors.size(); i++)
    {
        rlt[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    return rlt;
}

RenderNode::RenderNode(Entity* entity) : Component(entity), m_attachment(), m_framebuffer(0), m_loopIn(nullptr), m_loopOut(nullptr), m_drawFlag(false)
{
}

RenderNode::~RenderNode()
{
    glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderNode::AddInputSource(const std::vector<RenderNode *>& nodes, const std::vector<std::pair<std::string, const Graphics::Texture*>>& textures, const Graphics::Mesh &mesh)
{
    ConfigureInputSourcesParameter(textures, mesh);
    m_sources.emplace_back(nodes, mesh);
}
void RenderNode::ConfigureInputSourcesParameter(const std::vector<std::pair<std::string, const Graphics::Texture*>>& textures, const Graphics::Mesh &mesh)
{
    auto material = mesh.GetMaterial();
    material->GetShader()->Use();
    auto param = material->GetParameters();
    size_t count = 0;
    for (auto iter : textures)
    {
        assert(param->SetParameter(iter.first, Graphics::TextureUnitBinding(count++, iter.second)) && "No such global parameter name");
    }
}

void RenderNode::SetRenderLoop(RenderNode *start, const std::vector<std::pair<std::string, const Graphics::Texture*>>& textures, const Graphics::Mesh &drawMesh, const size_t &loopCount)
{
    start->m_loopIn = std::make_unique<RenderLoopIn>(this, drawMesh);
    start->ConfigureInputSourcesParameter(textures, start->m_loopIn->mesh);
    m_loopOut = std::make_unique<RenderLoopOut>(start, loopCount);
}
void RenderNode::GenFramebuffer()
{
    if (m_framebuffer != 0)glDeleteFramebuffers(1, &m_framebuffer);
    m_attachment.colors.clear();
    if(m_attachment.depth)m_attachment.depth->Free();
    if(m_attachment.stencil) m_attachment.stencil->Free();
    glGenFramebuffers(1, &m_framebuffer);
}

void RenderNode::AttachTexture2D(const AttachmentType &attachmentType,
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

void RenderNode::Use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

const GLuint &RenderNode::GetFramebuffer() const
{
    return m_framebuffer;
}
const RenderNode::FramebufferAttachment *RenderNode::GetAttachment() const
{
    return &m_attachment;
}
const std::vector<RenderNode::InputSource> *RenderNode::GetSource() const
{
    return &m_sources;
}

void RenderNode::UsePredefinedAttachments(const PredefinedAttachments &choice)
{
    switch (choice)
    {
    case PredefinedAttachments::HDR:
        AttachTexture2D(RenderNode::AttachmentType::Color,
                        Graphics::Texture::InternalFormat::RGBA16F,
                        Graphics::Texture::DataFormat::RGBA,
                        Graphics::Texture::DataType::HalfFloat,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        AttachTexture2D(RenderNode::AttachmentType::Depth,
                        Graphics::Texture::InternalFormat::DepthComponent,
                        Graphics::Texture::DataFormat::Depth,
                        Graphics::Texture::DataType::UnsignedInt,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        break;
    case PredefinedAttachments::Standard:
        AttachTexture2D(RenderNode::AttachmentType::Color,
                        Graphics::Texture::InternalFormat::RGBA8,
                        Graphics::Texture::DataFormat::RGBA,
                        Graphics::Texture::DataType::HalfFloat,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        AttachTexture2D(RenderNode::AttachmentType::Depth,
                        Graphics::Texture::InternalFormat::DepthComponent,
                        Graphics::Texture::DataFormat::Depth,
                        Graphics::Texture::DataType::UnsignedInt,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        break;
    }
}
RenderNode::RenderLoopIn *RenderNode::GetLoopIn() const
{
    return m_loopIn.get();
}
RenderNode::RenderLoopOut *RenderNode::GetLoopOut() const
{
    return m_loopOut.get();
}
void RenderNode::SetDrawFlag(const bool &draw)
{
    m_drawFlag = draw;
}
const bool &RenderNode::GetDrawFlag() const
{
    return m_drawFlag;
}
const Graphics::Texture* RenderNode::GetColorAttachment(const size_t& loc) const
{
    return m_attachment.colors[0].get();
}
const Graphics::Texture *RenderNode::GetDepthAttachment() const
{
    return m_attachment.depth.get();
}
const Graphics::Texture *RenderNode::GetStencilAttachment() const
{
    return m_attachment.stencil.get();
}
} // namespace wlEngine