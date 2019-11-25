#include "RenderNode.hpp"
#include "Shader.hpp"
#include "Mesh.hpp"
#include "Material.hpp"
#include "DebugDraw3D.hpp"

#include "../Component/Model.hpp"
#include "../EngineManager.hpp"
#include "../System/RenderSystem.hpp"

namespace wlEngine
{
namespace Graphics
{

RenderNode::InputSource::InputSource(RenderNode* node, const Mesh& mesh) : node(node), mesh(mesh){}
RenderNode::FramebufferAttachment::FramebufferAttachment() : colors(), depth(), stencil() {}
RenderNode::FramebufferAttachment::~FramebufferAttachment()
{}
std::vector<GLenum> RenderNode::FramebufferAttachment::GetColorAttachmentArray() const
{
    std::vector<GLenum> rlt(colors.size());
    for  (size_t i = 0; i < colors.size(); i++)
    {
        rlt[i] = GL_COLOR_ATTACHMENT0 + i;
    }
    return rlt;
}

RenderNode::RenderNode() : m_attachment(), m_framebuffer(0)
{
}

RenderNode::~RenderNode()
{
    glDeleteFramebuffers(1, &m_framebuffer);
}

void RenderNode::AddInputSource(RenderNode *node, const Mesh &mesh)
{
    sources.emplace_back(node, mesh);
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
                                 const unsigned int &height
                                 )
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
const RenderNode::FramebufferAttachment *RenderNode::Render() const
{
    if (sources.size() == 0)
        return RenderFromScene();

    //iteratively draw previous renderNode
    const std::string renderNodeColor_str = "renderNodeColor";
    const std::string renderNodeDepth_str = "renderNodeDepth";
    const std::string renderNodeStencil_str = "renderNodeBStencil";
    for (auto &inputSource : sources)
    {
        auto fbAttachment = inputSource.node->Render();
        auto material = inputSource.mesh.GetMaterial();
        material->UseShader();
        auto param = material->GetParameters();
        for (int i = 0; i < fbAttachment->colors.size(); i++)
        {
			param->SetParameter(renderNodeColor_str + std::to_string(i), TextureUnitBinding(2 + i, &(fbAttachment->colors[i])));
        }
        if (fbAttachment->depth.GetId() != 0)
        {
            param->SetParameter(renderNodeDepth_str, TextureUnitBinding(0, &fbAttachment->depth));
        }
        if (fbAttachment->depth.GetId() != 0)
        {
            param->SetParameter(renderNodeDepth_str, TextureUnitBinding(1, &fbAttachment->stencil));
        }

        //draw the output texture
        param->Use();
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear framebuffer
        auto sceneSize = wlEngine::RenderSystem::get()->getSceneSize();
        glViewport(0, 0, sceneSize.x, sceneSize.y);
        glDrawBuffers(m_attachment.colors.size(), m_attachment.GetColorAttachmentArray().data());
        glBindVertexArray(inputSource.mesh.GetVAO());
        glDrawElements(GL_TRIANGLES, inputSource.mesh.GetIndices()->size(), GL_UNSIGNED_INT, nullptr);
    }

    return &m_attachment;
}
const RenderNode::FramebufferAttachment *RenderNode::RenderFromScene() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(m_attachment.colors.size(), m_attachment.GetColorAttachmentArray().data());
    auto sceneSize = wlEngine::RenderSystem::get()->getSceneSize();
    glViewport(0, 0, sceneSize.x, sceneSize.y);
    RenderModel();
    return &m_attachment;
}
void RenderNode::RenderModel() const
{
    auto currentScene = wlEngine::EngineManager::getwlEngine()->getCurrentScene();
    for (auto model : wlEngine::Model::collection)
    {
        if (!model->entity->IsEnable() || model->entity->GetScene() != currentScene)
            continue;

        auto gameObject = model->entity;
        auto modelM = model->getModel();
        if (!modelM)
            return;
        for (auto mesh : *modelM->GetMeshes())
        {
            auto material = mesh.GetMaterial();
            auto shader = material->GetShader();
            shader->Use();
            material->GetParameters()->Use();
            //
            // draw mesh
            glBindVertexArray(mesh.GetVAO());
            if (shader->hasTess())
            {
                glPolygonMode(GL_FRONT_AND_BACK, model->GetDrawMode());
                glPatchParameteri(GL_PATCH_VERTICES, shader->getPatches());
                glDrawElements(GL_PATCHES, mesh.GetIndices()->size(), GL_UNSIGNED_INT, 0);
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else
            {
                glDrawElements(GL_TRIANGLES, mesh.GetIndices()->size(), GL_UNSIGNED_INT, 0);
            }
        }
        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);
#ifdef DEBUG
        auto aabb = modelM->getAABB();
        //small offset to prevent collision
        const float offset = 0.001;
        Graphics::DebugDraw3D::get()->drawBox(aabb.min, aabb.max, model->entity->GetComponent<Transform>()->getModel());
#endif
    }
}

} // namespace Graphics
} // namespace wlEngine