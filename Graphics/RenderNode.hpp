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
class Camera;
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

    RenderNode(Camera* camera);
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
    const Camera* GetCamera() const;
    const GLuint& GetFramebuffer() const;
    const FramebufferAttachment* GetAttachment() const;
    const std::vector<InputSource>* GetSource() const;

private:
    Camera* m_camera; //the attached camera
    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;

    std::vector<InputSource> m_sources;

    friend class RenderSystem;
};
} // namespace Graphics
} // namespace wlEngine