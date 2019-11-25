#include "RenderNode.hpp"
#include "Shader.hpp"

namespace wlEngine
{
namespace Graphics
{

FramebufferAttachment::FramebufferAttachment() : colors(), depth(0), stencil(0) {}
FramebufferAttachment::~FramebufferAttachment()
{
    glDeleteTextures(colors.size(), colors.data());
    glDeleteTextures(1, &depth);
    glDeleteTextures(1, &stencil);
}

RenderNode::RenderNode() : m_attachment()
{
    glGenFramebuffers(1, &m_framebuffer);
}

RenderNode::~RenderNode()
{
    glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderNode::BeginAttachment()
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
}

void RenderNode::EndAttachment()
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void RenderNode::AttachTexture2D(const AttachmentType &attachmentType,
                                 const TextureInternalFormat &internalFormat,
                                 const unsigned int &width,
                                 const unsigned int &height,
                                 const TextureDataFormat &format,
                                 const TextureDataType &type)
{
    GLuint texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, internalFormat, internalFormat, width, height, 0, format, type, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    if (attachmentType == AttachmentType::Color)
    {
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType + m_attachment.colors.size(),
                               GL_TEXTURE_2D,
                               texture,
                               0);
        m_attachment.colors.push_back(texture);
    }
    else if (attachmentType == AttachmentType::Depth)
    {
        if (m_attachment.depth != 0)
        {
            glDeleteTextures(1, &m_attachment.depth);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType,
                               GL_TEXTURE_2D,
                               texture,
                               0);
        m_attachment.depth = texture;
    }
    else if (attachmentType == AttachmentType::Stencil)
    {
        if (m_attachment.stencil != 0)
        {
            glDeleteTextures(1, &m_attachment.stencil);
        }
        glFramebufferTexture2D(GL_FRAMEBUFFER,
                               attachmentType,
                               GL_TEXTURE_2D,
                               texture,
                               0);
        m_attachment.stencil = texture;
    }
}

const FramebufferAttachment *RenderNode::Render(Model *model)
{

    return &m_attachment;
}
const FramebufferAttachment *RenderNode::Render(RenderNode * node)
{
    glDrawBuffers(m_attachment.colors.size(), m_attachment.colors.data());

    m_shader->Use();
    m_shaderParam.Use();

    std::string renderNode_str = "renderNodeBuffer";
    for (int i = 0; i < m_attachment.colors.size(); i++)
    {
        m_shaderParam.SetParameter(renderNode_str + std::to_string(i), m_attachment.colors[0]);
    }

    const float vertices[] = { //TODO: agile
        1.0
    };

    return &m_attachment;
}

void RenderNode::SetShader(Shader *shader)
{
    m_shader = shader;
    m_shaderParam.UpdateParameters(shader);
}
} // namespace Graphics
} // namespace wlEngine