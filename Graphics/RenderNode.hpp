#pragma once

#include "ShaderParameter.hpp"
#include "Mesh.hpp"
#include "Texture.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

namespace wlEngine
{
class RenderSystem;
namespace Graphics
{
class Shader;
class Model;
class RenderNode
{
public:
    enum AttachmentType
    {
        Color = GL_COLOR_ATTACHMENT0,
        Stencil = GL_STENCIL_ATTACHMENT,
        Depth = GL_DEPTH_ATTACHMENT,
    };
    struct InputSource
    {
        RenderNode* node;
        Mesh mesh;
        InputSource(RenderNode* node, const Mesh& mesh);
    };
    struct FramebufferAttachment
    {
        std::vector<Texture> colors;
        Texture depth;
        Texture stencil;
        std::vector<GLenum> GetColorAttachmentArray() const;
        FramebufferAttachment();
        ~FramebufferAttachment();
    };

    RenderNode();
    ~RenderNode();

    void AddInputSource(RenderNode* node, const Mesh& mesh);
    void GenFramebuffer();
    void AttachTexture2D(const AttachmentType &attachmentType,
                         const Texture::InternalFormat &internalFormat,
                         const Texture::DataFormat &format,
                         const Texture::DataType &type,
                         const unsigned int &width,
                         const unsigned int &height);
    void Use() const;

private:
    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;

    const FramebufferAttachment *Render() const;
    const FramebufferAttachment *RenderFromScene() const;
    void RenderModel() const;

    std::vector<InputSource> sources;

    friend class RenderSystem;
};
} // namespace Graphics
} // namespace wlEngine