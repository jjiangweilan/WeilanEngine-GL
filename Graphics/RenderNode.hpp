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
    struct RenderLoopIn;
    struct RenderLoopOut
    {
        RenderNode* in;
        size_t count;
        RenderLoopOut(RenderNode* in, const size_t& count) : in(in), count(count){}
    };

    RenderNode(Camera* camera);
    ~RenderNode();

    void AddInputSource(RenderNode* node, const Mesh& mesh);
    void ConfigureInputSourcesParameter(RenderNode* node, const Mesh& mesh);
    void GenFramebuffer();
    void AttachTexture2D(const AttachmentType &attachmentType,
                         const Texture::InternalFormat &internalFormat,
                         const Texture::DataFormat &format,
                         const Texture::DataType &type,
                         const unsigned int &width,
                         const unsigned int &height);
    void Use() const;
    /**
     * @brief Set the Render Loop object
     * 
     * @param start 
     * @param drawMesh the mesh used to draw the texture from start
     * @param loop 
     */
    void SetRenderLoop(RenderNode* start, const Graphics::Mesh& drawMesh, const size_t& loop);
    RenderLoopIn* GetLoopIn() const;
    RenderLoopOut* GetLoopOut() const;

    const Camera* GetCamera() const;
    const GLuint& GetFramebuffer() const;
    const FramebufferAttachment* GetAttachment() const;
    const std::vector<InputSource>* GetSource() const;

    enum class PredefinedAttachments
    {
        HDR, Bloom, Deferred, Standard
    };
    void UsePredefinedAttachments(const PredefinedAttachments&); //TODO: this needs to be renamed
private:
    struct RenderLoopIn
    {
        bool firstTime;
        InputSource source;
        RenderLoopIn(RenderNode* out, const Graphics::Mesh& mesh) : source(out, mesh), firstTime(true){}
    };
    Camera* m_camera; //the attached camera
    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;

    std::unique_ptr<RenderLoopIn> loopIn;
    std::unique_ptr<RenderLoopOut> loopOut;

    std::vector<InputSource> m_sources;

    friend class RenderSystem;
};
} // namespace Graphics
} // namespace wlEngine