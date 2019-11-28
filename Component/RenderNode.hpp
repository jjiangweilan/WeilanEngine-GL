#pragma once
#include "Component.hpp"

#include "../Graphics/Texture.hpp"
#include "../Graphics/Mesh.hpp"
namespace wlEngine
{
namespace Graphics
{
class Shader;
class Model;
}
class RenderNode : public Component
{
    COMPONENT_DECLARATION_NEW(Component, RenderNode);

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
        Graphics::Mesh mesh;
        InputSource(RenderNode* node, const Graphics::Mesh& mesh);
    };
    struct FramebufferAttachment
    {
        std::vector<Graphics::Texture> colors;
        Graphics::Texture depth;
        Graphics::Texture stencil;
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

    RenderNode(Entity *entity);
    ~RenderNode();

    void AddInputSource(RenderNode* node, const Graphics::Mesh& mesh);
    void ConfigureInputSourcesParameter(RenderNode* node, const Graphics::Mesh& mesh);
    void GenFramebuffer();
    void AttachTexture2D(const AttachmentType &attachmentType,
                         const Graphics::Texture::InternalFormat &internalFormat,
                         const Graphics::Texture::DataFormat &format,
                         const Graphics::Texture::DataType &type,
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
    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;

    std::unique_ptr<RenderLoopIn> loopIn;
    std::unique_ptr<RenderLoopOut> loopOut;

    std::vector<InputSource> m_sources;

    friend class RenderSystem;
};
}