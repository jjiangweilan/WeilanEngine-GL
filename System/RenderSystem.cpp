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

#include "../Graphics/Mesh.hpp"
#include "../Graphics/Material.hpp"
#include "../Graphics/Mesh2D.hpp"
#include "../Graphics/Model.hpp"

#include "../GameEditor/GameEditor.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_opengl3.h"

namespace wlEngine
{
RenderSystem *RenderSystem::renderSystem = nullptr;

RenderSystem::RenderSystem() : FramebufferSize(2), framebuffers(FramebufferSize), framebufferTextures(FramebufferSize), depthAndStencilTextures(FramebufferSize)
{
    // SDL and OpenGL init
    SDLInit();
    ImGuiInit();
    initSceneFrambufferData();
    buildInResourceInit();


    sceneShader = Shader::collection["Scene"];

    gameEditor = new GameEditor;
    for (int i = 0; i < FramebufferSize; i++)
    {
        genFramebuffer(framebuffers[i], framebufferTextures[i], depthAndStencilTextures[i]);
    }

#if SETTINGS_GAME_DIMENSION == 1
    projection = glm::perspective(glm::radians(45.0f), (float)sceneWidth / sceneHeight, 0.1f, 100000.0f);
#endif
#ifdef DEBUG
    Shader::loadShader("PhysicsDebugDrawShader", ROOT_DIR + "/Graphics/Material/Shader/PhysicsDebugDraw.vert", ROOT_DIR + "/Graphics/Material/Shader/PhysicsDebugDraw.frag");
    physicsDebugDrawShader = Shader::collection["PhysicsDebugDrawShader"];
    glGenVertexArrays(1, &physicsDebugVAO);
    glGenBuffers(1, &physicsDebugVBO);
#endif


    //uniform buffers
    glGenBuffers(1, &m_projectionUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
    glBufferData(GL_UNIFORM_BUFFER, 2 * sizeof(glm::mat4), nullptr, GL_STATIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
    glBindBufferBase(GL_UNIFORM_BUFFER, UNIFORM_BLOCK_INDEX_PROJECTION_MATRICS, m_projectionUBO);
    registerSystem(this);
}

RenderSystem::~RenderSystem()
{
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

#if SETTINGS_ENGINEMODE
    glDeleteFramebuffers(1, &sceneFramebuffer);
    glDeleteTextures(1, &sceneTexture);
    glDeleteTextures(1, &depthAndStencilTexture);
#endif
}

RenderSystem *RenderSystem::get() { return renderSystem; }

void RenderSystem::init() { renderSystem = new RenderSystem(); }

void RenderSystem::render()
{
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[FramebuffersIndex::Main]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    glViewport(0, 0, sceneWidth, sceneHeight);
    renderGame();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[FramebuffersIndex::VolumetricLight]);
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    glViewport(0, 0, sceneWidth, sceneHeight);
    glBlendFunc(GL_ONE, GL_ONE);
    for (auto c : VolumetricLight::collection)
    {
        if (!c->entity->isEnable())
            continue;
        render(c);
    }
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glBlendFunc(GL_ONE, GL_ZERO);
#if SETTINGS_ENGINEMODE
    //glBindFramebuffer(GL_FRAMEBUFFER, sceneFramebuffer);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    //glViewport(0, 0, sceneWidth, sceneHeight);
    //renderGame();
    combineTheFramebuffersToFramebuffer(sceneFramebuffer);
#ifdef DEBUG
    if (Settings::debugRender == Settings::DebugRender::On)
        debugRender();
#endif
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear main frambuffer
    glViewport(0, 0, windowWidth, windowHeight);
    renderGameEditor();
#else
    combineTheFramebuffersToFramebuffer(0);
#endif

    SDL_GL_SwapWindow(window);
}

#ifdef DEBUG
void RenderSystem::debugRender()
{
    for (auto &rb : TRigidbody::collection)
    {
        if (!rb->entity->isEnable())
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
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
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
            physicsDebugDrawShader->setMat4("projection",m_projMatrix);
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
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
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
            physicsDebugDrawShader->setMat4("projection",m_projMatrix);
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
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
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
            physicsDebugDrawShader->setMat4("projection",m_projMatrix);
            glm::vec3 color;
            color = {0.3, 0.8, 0.8};
            physicsDebugDrawShader->setVec3("color", glm::vec3(color.r, color.g, color.b));

            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
    }
}
#endif

void RenderSystem::update()
{
    updateFrameSettings();
    render();
}

void RenderSystem::updateFrameSettings() 
{
    //why we get the Camera2D then the Camera3D, instead of get the Camera?
    //this is a trick to force the isComponentRegs of Camera2D and Camera3D to initialize
    //the program will use dynamic initialization if no function of the class is called within the program (won't count lambda), this is wired...
    camera = m_engine->getCurrentScene()->getCamera()->getComponent<Camera2D>();
    if (!camera) camera = m_engine->getCurrentScene()->getCamera()->getComponent<Camera3D>();
    
    m_viewMatrix = camera->getViewMatrix();
    m_projMatrix = camera->getProjMatrix();

    glBindBuffer(GL_UNIFORM_BUFFER, m_projectionUBO);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(glm::mat4), &m_viewMatrix[0][0]);
    glBufferSubData(GL_UNIFORM_BUFFER, sizeof(glm::mat4), sizeof(glm::mat4), &m_projMatrix[0][0]);
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
#if SETTINGS_ENGINEMODE
    //scene frame buffer
    genFramebuffer(sceneFramebuffer, sceneTexture, depthAndStencilTexture);
#endif

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
void RenderSystem::renderGameEditor()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    ImGui::NewFrame();

    void *data[3];
    data[0] = &sceneTexture;
    data[1] = &sceneWidth;
    data[2] = &sceneHeight;
    gameEditor->render(data);

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
        if (!c->entity->isEnable() || c->entity->getScene() != currentScene)
            continue;
        render(c);
    }

    for (auto c : Sprite::collection)
    {
        if (!c->entity->isEnable() || c->entity->getScene() != currentScene)
            continue;
        render(c);
    }

    for (auto text : Text::collection)
    {
        if (!text->entity->isEnable())
            continue;
        render(text);
    }
}
void RenderSystem::render(VolumetricLight *vl)
{
    auto vlShader = vl->getShader();
    auto mesh = vl->getMesh();
    auto& vlTextures = *mesh->getTextures();
    vlShader->use();
    auto transform = vl->entity->getComponent<Transform>();
    vlShader->setMat4("model", transform->getModel());
    vlShader->setMat4("view", m_viewMatrix);
    vlShader->setMat4("projection", m_projMatrix);

    int t = vlTextures[0]->getId();
    glBindVertexArray(mesh->getVAO());
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
void RenderSystem::render(Text *t)
{
    t->getShader()->use();
    //
    // main texture
    glActiveTexture(GL_TEXTURE0);
    auto model = t->entity->getComponent<Transform>()->getModel();
    size_t till = t->renderUntilCharacter();
    for (size_t i = 0; i < till; i++)
    {
        auto &characterETX = t->text[i];
        auto mesh = characterETX.character->getMesh();
        glBindTexture(GL_TEXTURE_2D, mesh->getTextures()->at(0)->getId());

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

    t->getShader()->use();
    if (t->beforeRenderFunc)
        t->beforeRenderFunc();

    auto mesh = t->getMesh();
    auto textures = mesh->getTextures();
    for (int i = 0; i < textures->size(); i++)
    {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures->at(i)->getId());
        Shader::setUniform(1000 + i, i);
    }

    auto animation = t->entity->getComponent<Animation>();
    auto tRigidbody = t->entity->getComponent<TRigidbody>();
    auto transform = t->entity->getComponent<Transform>();
    if (animation)
        t->getMesh()->clip(animation->getCurrentClip());

    //set the uniform values according to the shader
    Shader::setUniform(0, transform->getModel());
    Shader::setUniform(1, m_viewMatrix);
    Shader::setUniform(2, m_projMatrix);
    Shader::setUniform(7, t->transparency);
    if (tRigidbody)
    {
        Shader::setUniform(3, 1); // shaTRigidbody;
        glm::vec2 pos = transform->position;
        Shader::setUniform(4, tRigidbody->shape->leftPoint + pos);
        Shader::setUniform(5, tRigidbody->shape->rightPoint + pos);
    }
    else
    {
        Shader::setUniform(3, 0); // shaTRigidbody;
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

    if (model->beforeRenderFunc)
        model->beforeRenderFunc();
    auto transform = gameObject->getComponent<Transform>();
    auto modelMatrix = transform->getModel();

  //  auto shader = model->getShader();
  //  shader->use();

	auto modelM = model->getModel();
	if (!modelM) return;
    for (auto mesh : *modelM->getMeshes())
    {
        // bind appropriate textures
        unsigned int diffuseNr = 900;
        unsigned int specularNr = 910;
        unsigned int normalNr = 930;
        unsigned int heightNr = 940;
        mesh.m_material->useShader();
        Shader::setUniform(0, modelMatrix); //model
        Shader::setUniform(12, camera->entity->getComponent<Transform>()->position);
        auto& textures = *mesh.m_material->getTextures();
        for (unsigned int i = 0; i < textures.size(); i++)
        {
            glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
            // retrieve texture number (the N in diffuse_textureN)
            
            auto& type = textures[i]->getType();
            int loc = -1;
            //this should be converted into enum and integer
            switch (type)
            {
            case TextureType::Diffuse:
                loc = diffuseNr++;
                break;
            case TextureType::Specular:
                loc = specularNr++;
            case TextureType::Normals:
                loc = normalNr++;
            case TextureType::Height:
                loc = heightNr++;
            default:
                assert(0 && "texture type not surrpoted");
            }

            // now set the sampler to the correct texture unit
            Shader::setUniform(loc, i);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i]->getId());
        }
        //
        // draw mesh
        glBindVertexArray(mesh.VAO);
        auto shader = mesh.m_material->getShader();
		if (shader->hasTess())
		{
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			glPatchParameteri(GL_PATCH_VERTICES, 2);
			glDrawElements(GL_PATCHES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
        else
        {
            glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
        }

        glBindVertexArray(0);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, 0);

        // always good practice to set everything back to defaults once configured.
        if (model->afterRenderFunc)
            model->afterRenderFunc();

    }
}

void RenderSystem::genFramebuffer(GLuint &fb, GLuint &ft, GLuint &ds)
{
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);

    glGenTextures(1, &ft);
    glBindTexture(GL_TEXTURE_2D, ft);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, sceneWidth, sceneHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ft, 0);

    glGenTextures(1, &ds);
    glBindTexture(GL_TEXTURE_2D, ds);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, sceneWidth, sceneHeight, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, ds, 0);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_STENCIL_TEST);
    glEnable(GL_BLEND);
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
    sceneShader->use();
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

void RenderSystem::postInit()
{
    m_engine = EngineManager::getwlEngine();
}

void RenderSystem::buildInResourceInit()
{
    //shader
    Shader::loadShader("Scene", ROOT_DIR + "/Graphics/Material/Shader/Scene.vert", ROOT_DIR + "/Graphics/Material/Shader/Scene.frag");
    Shader::loadShader("Basic", ROOT_DIR + "/Graphics/Material/Shader/Basic.vert", ROOT_DIR + "/Graphics/Material/Shader/Basic.frag");
    Shader::loadShader("Sprite", ROOT_DIR + "/Graphics/Material/Shader/Sprite.vert", ROOT_DIR + "/Graphics/Material/Shader/Sprite.frag");
    Shader::loadShader("Text", ROOT_DIR + "/Graphics/Material/Shader/Text.vert", ROOT_DIR + "/Graphics/Material/Shader/Text.frag");
    Shader::loadShader("Model", ROOT_DIR + "/Graphics/Material/Shader/Model.vert", ROOT_DIR + "/Graphics/Material/Shader/Model.frag");
}
} // namespace wlEngine
