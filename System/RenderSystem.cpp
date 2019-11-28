#define GL_GLEXT_PROTOTYPES
#include "RenderSystem.hpp"

#include "../EngineManager.hpp"

#include "../Component/Sprite.hpp"
#include "../Component/Transform.hpp"
#include "../Component/Animation.hpp"
#include "../Component/Model.hpp"
#include "../Component/Text.hpp"
#include "../Component/TRigidbody.hpp"
#include "../Component/VolumetricLight.hpp"
#include "../Component/Camera2D.hpp"
#include "../Component/Camera3D.hpp"
#include "../Component/RenderScript.hpp"

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Material.hpp"
#include "../Graphics/Mesh2D.hpp"
#include "../Graphics/Model.hpp"
#include "../Graphics/DebugDraw3D.hpp"
#include "../Graphics/RenderNode.hpp"

#include "../GameEditor/GameEditor.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_opengl3.h"


namespace wlEngine
{
RenderSystem *RenderSystem::renderSystem = nullptr;

RenderSystem::RenderSystem() 
{
    // SDL and OpenGL init
    SDLInit();
    ImGuiInit();
    initSceneFrambufferData();

    sceneShader = Graphics::Shader::get("Scene");
    auto shaderPath = ROOT_DIR + "/Graphics/Material/Shader/";
    Graphics::Shader::add("Text", ROOT_DIR + "/Graphics/Material/Shader/Text.vert", ROOT_DIR + "/Graphics/Material/Shader/Text.frag");

#ifdef DEBUG
    physicsDebugDrawShader = Graphics::Shader::add("PhysicsDebugDrawShader",
                                                   ROOT_DIR + "/Graphics/Material/Shader/PhysicsDebugDraw.vert",
                                                   ROOT_DIR + "/Graphics/Material/Shader/PhysicsDebugDraw.frag");
    glGenVertexArrays(1, &physicsDebugVAO);
    glGenBuffers(1, &physicsDebugVBO);
#endif

    //uniform buffers
    registerSystem(this);
}

RenderSystem::~RenderSystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();
}

RenderSystem *RenderSystem::Get() { return renderSystem; }

void RenderSystem::init() { renderSystem = new RenderSystem(); }

void RenderSystem::render()
{
    auto renderNode = m_mainCamera->GetRenderNode();
    auto attachment = renderNode->GetAttachment();
    Render(renderNode);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear main frambuffer
    glViewport(0, 0, windowWidth, windowHeight);
    unsigned int id = attachment->colors[0].GetId();
    renderGameEditor(id);

    SDL_GL_SwapWindow(window);
}


void RenderSystem::update()
{
    render();
}

int RenderSystem::windowResizeCallbackWrapper(void *data, SDL_Event *event)
{
    if (renderSystem)
        return renderSystem->windowResizeCallback(data, event);
    return 0;
}

int RenderSystem::windowResizeCallback(void *data, SDL_Event *event)
{
    if (event->type == SDL_WINDOWEVENT)
    {
        switch (event->window.event)
        {
        case SDL_WINDOWEVENT_RESIZED:
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            break;
        }
    }

    return 0;
}

void RenderSystem::setViewPort(int x, int y, int width, int height) { glViewport(x, y, width, height); }

void RenderSystem::SDLInit()
{
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
    SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
    stbi_set_flip_vertically_on_load(true);

    int SDL_Flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#if SETTINGS_ENGINEMODE == 0
    windowWidth = sceneWidth;
    windowHeight = sceneHeight;
#endif
    window = SDL_CreateWindow("uuen's tale", 150, 50, windowWidth, windowHeight, SDL_Flags);
    glContext = SDL_GL_CreateContext(window);
    SDL_AddEventWatch(windowResizeCallbackWrapper, window);
    gladLoadGLLoader(SDL_GL_GetProcAddress);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glClearStencil(0);
}

void RenderSystem::ImGuiInit()
{
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, &glContext);

    ImGui_ImplOpenGL3_Init("#version 450");
}
#if SETTINGS_ENGINEMODE
void RenderSystem::renderGameEditor(unsigned int& sceneTexId)
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    void *data[3];
    data[0] = &sceneTexId;
    data[1] = &sceneWidth;
    data[2] = &sceneHeight;
    GameEditor::get()->render(data);

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif

/* Render *************/
void RenderSystem::renderGame()
{
    auto currentScene = EngineManager::getwlEngine()->getCurrentScene();

    for (auto c : Model::collection)
    {
        if (!c->entity->IsEnable() || c->entity->GetScene() != currentScene)
            continue;
        render(c);
    }

    for (auto c : Sprite::collection)
    {
        if (!c->entity->IsEnable() || c->entity->GetScene() != currentScene)
            continue;
        render(c);
    }

    for (auto text : Text::collection)
    {
        if (!text->entity->IsEnable())
            continue;
        render(text);
    }
}
void RenderSystem::render(VolumetricLight *vl)
{
    auto vlShader = vl->getShader();
    auto mesh = vl->getMesh();
    auto &vlTextures = *mesh->getTextures();
    vlShader->Use();
    auto transform = vl->entity->GetComponent<Transform>();
    vlShader->setMat4("model", transform->getModel());
    vlShader->setMat4("view", m_viewMatrix);
    vlShader->setMat4("projection", m_projMatrix);

    int t = vlTextures[0]->GetId();
    glBindVertexArray(mesh->getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
void RenderSystem::render(Text *t)
{
    t->getShader()->Use();
    //
    // main texture
    glActiveTexture(GL_TEXTURE0);
    auto model = t->entity->GetComponent<Transform>()->getModel();
    size_t till = t->renderUntilCharacter();
    for (size_t i = 0; i < till; i++)
    {
        auto &characterETX = t->text[i];
        auto mesh = characterETX.character->getMesh();
        glBindTexture(GL_TEXTURE_2D, mesh->getTextures()->at(0)->GetId());

        t->getShader()->setMat4("model", model * characterETX.getTextTransform());
        t->getShader()->setMat4("view", m_viewMatrix);
        t->getShader()->setMat4("projection", m_projMatrix);
        glBindVertexArray(mesh->getVAO());
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // glBindTexture(GL_TEXTURE_2D, 0);
    // glBindVertexArray(0);
}

void RenderSystem::render(Sprite *t)
{
    if (!t->enable)
        return;

    if (t->draw)
    {
        t->draw(m_viewMatrix, m_projMatrix);
        return;
    }

    t->getShader()->Use();
    if (t->beforeRenderFunc)
        t->beforeRenderFunc();

    auto mesh = t->getMesh();
    auto textures = mesh->getTextures();
    for (int i = 0; i < textures->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures->at(i)->GetId());
        Graphics::Shader::setUniform(1000 + i, i);
    }

    auto animation = t->entity->GetComponent<Animation>();
    auto tRigidbody = t->entity->GetComponent<TRigidbody>();
    auto transform = t->entity->GetComponent<Transform>();
    if (animation)
        t->getMesh()->clip(animation->getCurrentClip());

    //set the uniform values according to the shader
    Graphics::Shader::setUniform(0, transform->getModel());
    Graphics::Shader::setUniform(1, m_viewMatrix);
    Graphics::Shader::setUniform(2, m_projMatrix);
    Graphics::Shader::setUniform(7, t->transparency);
    if (tRigidbody)
    {
        Graphics::Shader::setUniform(3, 1); // shaTRigidbody;
        glm::vec2 pos = transform->position;
        Graphics::Shader::setUniform(4, tRigidbody->shape->leftPoint + pos);
        Graphics::Shader::setUniform(5, tRigidbody->shape->rightPoint + pos);
    }
    else
    {
        Graphics::Shader::setUniform(3, 0); // shaTRigidbody;
    }
    glBindVertexArray(mesh->getVAO());

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if (t->afterRenderFunc)
        t->afterRenderFunc();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
}

void RenderSystem::render(Model *model)
{
    auto gameObject = model->entity;
    auto modelM = model->getModel();
    if (!modelM)
        return;
    for (auto mesh : *modelM->GetMeshes())
    {
        auto shader = mesh.m_material->GetShader();
		shader->Use();
        mesh.m_material->GetParameters()->Use();
        //
        // draw mesh
        glBindVertexArray(mesh.VAO);
        if (shader->hasTess())
        {
            glPolygonMode(GL_FRONT_AND_BACK, model->GetDrawMode());
            glPatchParameteri(GL_PATCH_VERTICES, shader->getPatches());
            glDrawElements(GL_PATCHES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        else
        {
            glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
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

void RenderSystem::initSceneFrambufferData()
{
    GLfloat vertices[] = {
        1.0, 1.0, 1.0f, 1.0f,   // top right
        1.0, -1.0, 1.0f, 0.0f,  // bottom right
        -1.0, -1.0, 0.0f, 0.0f, // bottom left
        -1.0, 1.0, 0.0f, 1.0f   // top left
    };

    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };

    glGenVertexArrays(1, &sceneVAO);
    glGenBuffers(1, &sceneVBO);
    glGenBuffers(1, &sceneEBO);
    glBindVertexArray(sceneVAO);
    glBindBuffer(GL_ARRAY_BUFFER, sceneVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sceneEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_DYNAMIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void RenderSystem::combineTheFramebuffersToFramebuffer(const GLuint &framebufferTarget)
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferTarget);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    glViewport(0, 0, sceneWidth, sceneHeight);
    sceneShader->Use();
    // draw the main scene
    glBindVertexArray(sceneVAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, framebufferTextures[FramebuffersIndex::Main]);
    sceneShader->setInt("gameSceneSampler", 0);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, framebufferTextures[FramebuffersIndex::VolumetricLight]);
    sceneShader->setInt("volumetricLightSampler", 1);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void RenderSystem::PostInit()
{
    m_engine = EngineManager::getwlEngine();
}


void RenderSystem::Render(Graphics::RenderNode* node) 
{
    auto sources = node->GetSource();
    auto loopIn = node->GetLoopIn();
    auto loopOut = node->GetLoopOut();

    //This is a looped render path
    if (loopOut)
    {
        size_t count = loopOut->count;
        while(count--)
        {
            RenderInputSources(node);
        }
        loopOut->in->GetLoopIn()->firstTime = true;
        return;
    }

    if (loopIn)
    {
        if (loopIn->firstTime){
            loopIn->firstTime = false;
            if (sources->size() == 0) return RenderFromScene(node);
            RenderInputSources(node);
        }
        else
        {
            //only has one frame buffer so we don't call RenderInputSources, because
            //the only source is the loopIn->source
            auto renderScript = node->GetCamera()->entity->GetComponent<RenderScript>();
            if (renderScript)
                renderScript->Update();
            RenderToFramebuffer(node, &loopIn->source.mesh);
        }
        return;
    }
    

    //no loop
    if (sources->size() == 0)
        return RenderFromScene(node);

    RenderInputSources(node);
}
void RenderSystem::RenderInputSources(Graphics::RenderNode *node)
{
    auto sources = node->GetSource();
    auto renderScript = node->GetCamera()->entity->GetComponent<RenderScript>();
    if (renderScript)
        renderScript->Update();
    for (auto &inputSource : *sources)
    {
        Render(inputSource.node);
        RenderToFramebuffer(node, &inputSource.mesh);
    }
}
void RenderSystem::RenderToFramebuffer(Graphics::RenderNode* node, const Graphics::Mesh *mesh)
{
    auto attachment = node->GetAttachment();
    mesh->GetMaterial()->UseMaterial();
    glBindFramebuffer(GL_FRAMEBUFFER, node->GetFramebuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear framebuffer
    auto sceneSize = wlEngine::RenderSystem::Get()->GetSceneSize();
    glViewport(0, 0, sceneSize.x, sceneSize.y);
    glDrawBuffers(attachment->colors.size(), attachment->GetColorAttachmentArray().data());
    glBindVertexArray(mesh->GetVAO());
    glDrawElements(GL_TRIANGLES, mesh->GetIndices()->size(), GL_UNSIGNED_INT, nullptr);
}
void RenderSystem::RenderFromScene(Graphics::RenderNode *node)
{
    auto attachment = node->GetAttachment();
    auto renderScript = node->GetCamera()->entity->GetComponent<RenderScript>();
    if (renderScript)
        renderScript->Update();

    glBindFramebuffer(GL_FRAMEBUFFER, node->GetFramebuffer());
    glClear(GL_COLOR_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawBuffers(attachment->colors.size(), attachment->GetColorAttachmentArray().data());
    auto sceneSize = wlEngine::RenderSystem::Get()->GetSceneSize();
    glViewport(0, 0, sceneSize.x, sceneSize.y);
    RenderModel(node);
}
void RenderSystem::RenderModel(Graphics::RenderNode *node)
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
        auto renderScript = model->entity->GetComponent<RenderScript>();
        if (renderScript)
            renderScript->Update();

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

#ifdef DEBUG
void RenderSystem::debugRender()
{
    for (auto &rb : TRigidbody::collection)
    {
        if (!rb->entity->IsEnable())
            continue;
        auto shape = rb->shape->getShapeType();
        if (shape == ShapeType::Polygon || shape == ShapeType::Line)
        {
            std::vector<glm::vec2> vertices;
            if (shape == ShapeType::Polygon)
                vertices = static_cast<PolygonShape *>(rb->shape)->getPoints();
            else
                vertices = static_cast<LineShape *>(rb->shape)->getPoints();
            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->Use();
            auto pos = rb->entity->GetComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++)
            {
                glVertices[2 * i] = vertices[i].x + pos.x;
                glVertices[2 * i + 1] = vertices[i].y + pos.y;
            }

            glBindVertexArray(physicsDebugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, physicsDebugVBO);
            glBufferData(GL_ARRAY_BUFFER, glVertices.size() * sizeof(float), glVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);

            glm::mat4 proj = glm::mat4(1.0f);

#if SETTINGS_GAME_DIMENSION
            proj = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100000.0f);

#else
            proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1000.0f);
#endif
            physicsDebugDrawShader->setMat4("view", m_viewMatrix);
            physicsDebugDrawShader->setMat4("projection", m_projMatrix);
            glm::vec3 color;
            if (rb->type == BodyType::Dynamic)
                color = {1, 0, 0};
            else
                color = {0, 1, 0};
            physicsDebugDrawShader->setVec3("color", glm::vec3(color.r, color.g, color.b));
            if (shape == ShapeType::Polygon)
                glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
            else
                glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
        else if (shape == ShapeType::Circle)
        {
            auto circle = static_cast<CircleShape *>(rb->shape);
            float centerX = circle->center.x;
            float centerY = circle->center.y;
            float radius = circle->radius; // 1.41421356237 ~ square root of 2
            std::vector<glm::vec2> vertices = {{centerX + radius, centerY + radius}, {centerX + radius, centerY - radius}, {centerX - radius, centerY - radius}, {centerX - radius, centerY + radius}};

            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->Use();
            auto pos = rb->entity->GetComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++)
            {
                glVertices[2 * i] = vertices[i].x + pos.x;
                glVertices[2 * i + 1] = vertices[i].y + pos.y;
            }

            glBindVertexArray(physicsDebugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, physicsDebugVBO);
            glBufferData(GL_ARRAY_BUFFER, glVertices.size() * sizeof(float), glVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);

            glm::mat4 proj = glm::mat4(1.0f);

#if SETTINGS_GAME_DIMENSION
            proj = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100000.0f);

#else
            proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1000.0f);
#endif
            physicsDebugDrawShader->setMat4("view", m_viewMatrix);
            physicsDebugDrawShader->setMat4("projection", m_projMatrix);
            glm::vec3 color;
            if (rb->type == BodyType::Dynamic)
                color = {1, 0, 0};
            else
                color = {0, 1, 0};
            physicsDebugDrawShader->setVec3("color", glm::vec3(color.r, color.g, color.b));

            glDrawArrays(GL_LINE_LOOP, 0, vertices.size());
        }

        // render render line
        {
            auto shape = rb->shape;
            std::vector<glm::vec2> vertices = {shape->leftPoint, shape->rightPoint};

            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->Use();
            auto pos = rb->entity->GetComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++)
            {
                glVertices[2 * i] = vertices[i].x + pos.x;
                glVertices[2 * i + 1] = vertices[i].y + pos.y;
            }

            glBindVertexArray(physicsDebugVAO);
            glBindBuffer(GL_ARRAY_BUFFER, physicsDebugVBO);
            glBufferData(GL_ARRAY_BUFFER, glVertices.size() * sizeof(float), glVertices.data(), GL_STATIC_DRAW);
            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
            glEnableVertexAttribArray(0);

            glm::mat4 proj = glm::mat4(1.0f);

#if SETTINGS_GAME_DIMENSION
            proj = glm::perspective(glm::radians(45.0f), (float)windowWidth / windowHeight, 0.1f, 100000.0f);

#else
            proj = glm::ortho(0.0f, (float)windowWidth, 0.0f, (float)windowHeight, -1.0f, 1000.0f);
#endif
            physicsDebugDrawShader->setMat4("view", m_viewMatrix);
            physicsDebugDrawShader->setMat4("projection", m_projMatrix);
            glm::vec3 color;
            color = {0.3, 0.8, 0.8};
            physicsDebugDrawShader->setVec3("color", glm::vec3(color.r, color.g, color.b));

            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
    }
}
void RenderSystem::SetOutputCamera(Camera *camera)
{
    m_mainCamera = camera;
}
#endif
} // namespace wlEngine
