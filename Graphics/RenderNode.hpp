#pragma once

#include "ShaderParameter.hpp"

#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>

namespace wlEngine
{
namespace Graphics
{
enum TextureInternalFormat
{

};
enum TextureDataFormat
{

};
enum TextureDataType
{

};
enum AttachmentType
{
    Color = GL_COLOR_ATTACHMENT0,
    Stencil = GL_STENCIL_ATTACHMENT,
    Depth = GL_DEPTH_ATTACHMENT,
};
struct FramebufferAttachment
{
    std::vector<GLuint> colors;
    GLuint depth;
    GLuint stencil;
    FramebufferAttachment();
    ~FramebufferAttachment();
};
class Shader;
class Model;
class RenderNode
{
public:
    RenderNode();
    ~RenderNode();

    const FramebufferAttachment *Render(Model *);
    const FramebufferAttachment *Render(RenderNode *);
    void GenFramebuffer();
    void BeginAttachment();
    void EndAttachment();
    void AttachTexture2D(const AttachmentType &attachmentType,
                         const TextureInternalFormat &internalFormat,
                         const unsigned int &width,
                         const unsigned int &height,
                         const TextureDataFormat &format,
                         const TextureDataType &type);

    void SetShader(Shader* shader);
private:
    Shader *m_shader;
    ShaderParameter m_shaderParam;

    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;
};
} // namespace Graphics
} // namespace wlEngine