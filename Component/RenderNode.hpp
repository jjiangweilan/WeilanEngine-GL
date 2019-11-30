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
class Texture;
}
enum class PredefinedAttachmentType
{
    HDR,
    Standard,
    Depth
};
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
        std::vector<RenderNode *> nodes;
        Graphics::Mesh mesh;
        InputSource(const std::vector<RenderNode *> &nodes, const Graphics::Mesh &mesh);
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
    struct RenderLoopIn;
    struct RenderLoopOut

    {
        RenderNode *in;
        size_t count;
        RenderLoopOut(RenderNode *in, const size_t &count) : in(in), count(count) {}
    };

    RenderNode(Entity *entity);
    ~RenderNode();

    void AddInputSource(const std::vector<RenderNode *> &nodes,
                        const std::vector<
                            std::pair<std::string, const Graphics::Texture *>> &textures,
                        const Graphics::Mesh &mesh);

    void ConfigureInputSourcesParameter(const std::vector<
                                            std::pair<std::string, const Graphics::Texture *>> &textures,
                                        const Graphics::Mesh &mesh);
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
    void SetRenderLoop(RenderNode *start, const std::vector<std::pair<std::string, const Graphics::Texture *>> &textures, const Graphics::Mesh &drawMesh, const size_t &loop);
    RenderLoopIn *GetLoopIn() const;
    RenderLoopOut *GetLoopOut() const;

    const GLuint &GetFramebuffer() const;
    const FramebufferAttachment *GetAttachment() const;
    const Graphics::Texture *GetColorAttachment(const size_t &loc) const;
    const Graphics::Texture *GetDepthAttachment() const;
    const Graphics::Texture *GetStencilAttachment() const;
    const std::vector<InputSource> *GetSource() const;

    void SetDrawFlag(const bool &draw);
    const bool &GetDrawFlag() const;

private:
    struct RenderLoopIn
    {
        bool firstTime;
        RenderNode *out;
        Graphics::Mesh mesh;
        RenderLoopIn(RenderNode *out, const Graphics::Mesh &mesh) : out(out), mesh(mesh), firstTime(true) {}
    };
    GLuint m_framebuffer;
    FramebufferAttachment m_attachment;

    std::unique_ptr<RenderLoopIn> m_loopIn;
    std::unique_ptr<RenderLoopOut> m_loopOut;

    std::vector<InputSource> m_sources;
    bool m_drawFlag;

    friend class RenderSystem;
};
} // namespace wlEngine