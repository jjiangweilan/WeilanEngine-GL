#include "RenderNode.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "DebugDraw3D.hpp"
#include "GlobalShaderParameter.hpp"

#include "../Component/Model.hpp"
#include "../Component/Camera.hpp"
#include "../Component/Transform.hpp"
#include "../Component/RenderScript.hpp"

#include "../System/RenderSystem.hpp"
#include "../EngineManager.hpp"

namespace wlEngine
{
namespace Graphics
{

RenderNode::InputSource::InputSource(RenderNode *node, const Mesh &mesh) : node(node), mesh(mesh) {}
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

RenderNode::RenderNode(Camera *camera) : m_camera(camera), m_attachment(), m_framebuffer(0), loopIn(nullptr), loopOut(nullptr)
{
}

RenderNode::~RenderNode()
{
    glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderNode::AddInputSource(RenderNode *node, const Mesh &mesh)
{
    m_sources.emplace_back(node, mesh);
    ConfigureInputSourcesParameter(node, mesh);
}
void RenderNode::ConfigureInputSourcesParameter(RenderNode *node, const Mesh &mesh)
{

    const std::string renderNodeColor_str = "renderNodeColor";
    const std::string renderNodeDepth_str = "renderNodeDepth";
    const std::string renderNodeStencil_str = "renderNodeStencil";

    auto material = mesh.GetMaterial();
    material->GetShader()->Use();
    auto param = material->GetParameters();
    for (int i = 0; i < node->m_attachment.colors.size(); i++)
    {
        param->SetParameter(renderNodeColor_str + std::to_string(i), TextureUnitBinding(2 + i, &(node->m_attachment.colors[i])));
    }

    if (node->m_attachment.depth.GetId() != 0)
    {
        param->SetParameter(renderNodeDepth_str, TextureUnitBinding(0, &node->m_attachment.depth));
    }
    if (node->m_attachment.stencil.GetId() != 0)
    {
        param->SetParameter(renderNodeDepth_str, TextureUnitBinding(1, &node->m_attachment.stencil));
    }
}

void RenderNode::SetRenderLoop(RenderNode *start, const Graphics::Mesh &drawMesh, const size_t &loopCount)
{
    start->loopIn = std::make_unique<RenderLoopIn>(this, drawMesh);
    loopOut = std::make_unique<RenderLoopOut>(start, loopCount);
    start->ConfigureInputSourcesParameter(this, drawMesh);
}
void RenderNode::GenFramebuffer()
{
    glGenFramebuffers(1, &m_framebuffer);
}

void RenderNode::AttachTexture2D(const AttachmentType &attachmentType,
                                 const Texture::InternalFormat &internalFormat,
                                 const Texture::DataFormat &format,
                                 const Texture::DataType &type,
                                 const unsigned int &width,
                                 const unsigned int &height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);

    Texture texture(width, height, internalFormat, format, type);

    if (attachmentType == AttachmentType::Color)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType + m_attachment.colors.size(),
                               GL_TEXTURE_2D,
                               texture.GetId(),
                               0);
        m_attachment.colors.emplace_back(std::move(texture));
    }
    else if (attachmentType == AttachmentType::Depth)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType,
                               GL_TEXTURE_2D,
                               texture.GetId(),
                               0);
        m_attachment.depth = std::move(texture);
    }
    else if (attachmentType == AttachmentType::Stencil)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType,
                               GL_TEXTURE_2D,
                               texture.GetId(),
                               0);
        m_attachment.stencil = std::move(texture);
    }
    auto status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (status != GL_FRAMEBUFFER_COMPLETE)
    {
        std::cerr << "frame buffer not completed: " << status << std::endl;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderNode::Use() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}
const Camera *RenderNode::GetCamera() const
{
    return m_camera;
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
    glDeleteFramebuffers(1, &m_framebuffer);
    GenFramebuffer();
    switch (choice)
    {
    case PredefinedAttachments::HDR:
        AttachTexture2D(Graphics::RenderNode::AttachmentType::Color,
                        Graphics::Texture::InternalFormat::RGBA16F,
                        Graphics::Texture::DataFormat::RGBA,
                        Graphics::Texture::DataType::HalfFloat,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        AttachTexture2D(Graphics::RenderNode::AttachmentType::Depth,
                        Graphics::Texture::InternalFormat::DepthComponent,
                        Graphics::Texture::DataFormat::Depth,
                        Graphics::Texture::DataType::UnsignedInt,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        break;
    case PredefinedAttachments::Standard:
        AttachTexture2D(Graphics::RenderNode::AttachmentType::Color,
                        Graphics::Texture::InternalFormat::RGBA8,
                        Graphics::Texture::DataFormat::RGBA,
                        Graphics::Texture::DataType::HalfFloat,
                        wlEngine::RenderSystem::Get()->GetSceneSize().x,
                        wlEngine::RenderSystem::Get()->GetSceneSize().y);
        AttachTexture2D(Graphics::RenderNode::AttachmentType::Depth,
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
    return loopIn.get();
}
RenderNode::RenderLoopOut *RenderNode::GetLoopOut() const
{
    return loopOut.get();
}
} // namespace Graphics
} // namespace wlEngine