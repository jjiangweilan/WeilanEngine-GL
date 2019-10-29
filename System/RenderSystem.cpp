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

#include "../GameEditor/GameEditor.hpp"

#include "../imgui/imgui.h"
#include "../imgui/imgui_impl_sdl.h"
#include "../imgui/imgui_impl_opengl3.h"

namespace wlEngine {
RenderSystem *RenderSystem::renderSystem = nullptr;

RenderSystem::RenderSystem() : FramebufferSize(2), framebuffers(FramebufferSize), framebufferTextures(FramebufferSize), depthAndStencilTextures(FramebufferSize) {
    // SDL and OpenGL init
    SDLInit();
    ImGuiInit();
    initSceneFrambufferData();
    Shader::loadShader("SceneShader","../Shader/SceneShader.vert", "../Shader/SceneShader.frag");
    Shader::loadShader("BasicShader","../Shader/BasicShader.vert", "../Shader/BasicShader.frag");
    sceneShader = Shader::collection["SceneShader"];

    gameEditor = new GameEditor;
    for(int i = 0 ; i < FramebufferSize; i++) {
        genFramebuffer(framebuffers[i], framebufferTextures[i], depthAndStencilTextures[i]);
    }    

#if SETTINGS_GAME_DIMENSION == 1
    projection = glm::perspective(glm::radians(45.0f), (float)sceneWidth / sceneHeight, 0.1f, 100000.0f);
#endif
#ifdef DEBUG
    Shader::loadShader("PhysicsDebugDrawShader", "../Shader/debugDrawShader.vert", "../Shader/debugDrawShader.frag");
    physicsDebugDrawShader = Shader::collection["PhysicsDebugDrawShader"];
    glGenVertexArrays(1, &physicsDebugVAO);
    glGenBuffers(1, &physicsDebugVBO);
#endif

    registerSystem(this);
}

RenderSystem::~RenderSystem() {
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

void RenderSystem::render() {
#if SETTINGS_GAME_DIMENSION == 0
    camera2D = EngineManager::getwlEngine()->getCurrentScene()->getCamera()->getComponent<Camera2D>();
    coordTransform = camera2D->getTransformMatrix();
#endif
    glBindFramebuffer(GL_FRAMEBUFFER,framebuffers[FramebuffersIndex::Main]);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    glViewport(0, 0, sceneWidth, sceneHeight);
    renderGame();

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffers[FramebuffersIndex::VolumetricLight]);
    glClearColor(0.2f, 0.2f, 0.2f, 0.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear scene frame buffer
    glViewport(0, 0, sceneWidth, sceneHeight);
    for (auto c : VolumetricLight::collection) {
        if (!c->entity->isEnable()) continue;
        render(c);
    }
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

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
void RenderSystem::debugRender() {
    for (auto &rb : TRigidbody::collection) {
        if (!rb->entity->isEnable())
            continue;
        auto shape = rb->shape->getShapeType();
        if (shape == ShapeType::Polygon || shape == ShapeType::Line) {
            auto camera = EngineManager::getwlEngine()->getCurrentScene()->getCamera()->getComponent<Camera2D>();
            std::vector<glm::vec2> vertices;
            if (shape == ShapeType::Polygon)
                vertices = static_cast<PolygonShape *>(rb->shape)->getPoints();
            else
                vertices = static_cast<LineShape *>(rb->shape)->getPoints();
            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++) {
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
            physicsDebugDrawShader->setMat4("view", coordTransform.view);
            physicsDebugDrawShader->setMat4("projection", coordTransform.projection);
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

        } else if (shape == ShapeType::Circle) {
            auto camera = EngineManager::getwlEngine()->getCurrentScene()->getCamera()->getComponent<Camera2D>();
            auto circle = static_cast<CircleShape *>(rb->shape);
            float centerX = circle->center.x;
            float centerY = circle->center.y;
            float radius = circle->radius; // 1.41421356237 ~ square root of 2
            std::vector<glm::vec2> vertices = {{centerX + radius, centerY + radius}, {centerX + radius, centerY - radius}, {centerX - radius, centerY - radius}, {centerX - radius, centerY + radius}};

            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++) {
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
            physicsDebugDrawShader->setMat4("view", coordTransform.view);
            physicsDebugDrawShader->setMat4("projection", coordTransform.projection);
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
            auto camera = EngineManager::getwlEngine()->getCurrentScene()->getCamera()->getComponent<Camera2D>();
            auto shape = rb->shape;
            std::vector<glm::vec2> vertices = {shape->leftPoint, shape->rightPoint};

            std::vector<float> glVertices(vertices.size() * 2);
            physicsDebugDrawShader->use();
            auto pos = rb->entity->getComponent<Transform>()->position;
            for (int i = 0; i < vertices.size(); i++) {
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
            physicsDebugDrawShader->setMat4("view", coordTransform.view);
            physicsDebugDrawShader->setMat4("projection", coordTransform.projection);
            glm::vec3 color;
            color = {0.3, 0.8, 0.8};
            physicsDebugDrawShader->setVec3("color", glm::vec3(color.r, color.g, color.b));

            glDrawArrays(GL_LINE_STRIP, 0, vertices.size());
        }
    }
}
#endif

void RenderSystem::update() { render(); }

int RenderSystem::windowResizeCallbackWrapper(void *data, SDL_Event *event) {
    if (renderSystem)
        return renderSystem->windowResizeCallback(data, event);
    return 0;
}

int RenderSystem::windowResizeCallback(void *data, SDL_Event *event) {
    if (event->type == SDL_WINDOWEVENT) {
        switch (event->window.event) {
        case SDL_WINDOWEVENT_RESIZED:
            SDL_GetWindowSize(window, &windowWidth, &windowHeight);
            break;
        }
    }

    return 0;
}

void RenderSystem::setViewPort(int x, int y, int width, int height) { glViewport(x, y, width, height); }

void RenderSystem::SDLInit() {
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

void RenderSystem::ImGuiInit() {
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();

    ImGui::StyleColorsDark();
    ImGui_ImplSDL2_InitForOpenGL(window, &glContext);

    ImGui_ImplOpenGL3_Init("#version 450");
}
#if SETTINGS_ENGINEMODE
void RenderSystem::renderGameEditor() {
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
void RenderSystem::renderGame() {
    auto currentScene = EngineManager::getwlEngine()->getCurrentScene();

    for (auto c : Sprite::collection) {
        if (!c->entity->isEnable() || c->entity->getScene() != currentScene)
            continue;
        render(c);
    }

    for (auto text : Text::collection) {
        if (!text->entity->isEnable())
            continue;
        render(text);
    }
}
void RenderSystem::render(VolumetricLight* vl) {
    vl->shader->use();
    auto transform = vl->entity->getComponent<Transform>();
    vl->shader->setMat4("model", transform->getModel());
    vl->shader->setMat4("view", coordTransform.view);
    vl->shader->setMat4("projection", coordTransform.projection);
    glBindVertexArray(vl->textures[0].VAO);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, vl->textures[0].mTexture);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
void RenderSystem::render(Text *t) {
    t->shader->use();
    //
    // main texture
    glActiveTexture(GL_TEXTURE0);
    auto model = t->entity->getComponent<Transform>()->getModel();
    size_t till = t->renderUntilCharacter();
    for (size_t i = 0; i < till; i++) {
        auto &character = t->text[i];
        glBindTexture(GL_TEXTURE_2D, character.texture->mTexture);

        t->shader->setMat4("model", model * character.getTextTransform());
        t->shader->setMat4("view", coordTransform.view);
        t->shader->setMat4("projection", coordTransform.projection);
        glBindVertexArray(character.texture->VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    }

    // glBindTexture(GL_TEXTURE_2D, 0);
    // glBindVertexArray(0);
}

void RenderSystem::render(Sprite *t) {
    if (!t->enable)
        return;

    if (t->draw) {
        t->draw(coordTransform.view, coordTransform.projection);
        return;
    }

    int i = 0;
    t->shader->use();
    if (t->beforeRenderFunc)
        t->beforeRenderFunc();
    //
    // main texture
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, t->mainTexture->mTexture);
    auto animation = t->entity->getComponent<Animation>();
    auto tRigidbody = t->entity->getComponent<TRigidbody>();
    auto transform = t->entity->getComponent<Transform>();
    if (animation)
        t->mainTexture->clip(animation->getCurrentClip(), true);

    // other textures
    for (auto &texture : t->textures) {
        i++;
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, texture.second->mTexture);
        if (animation)
            texture.second->clip(animation->getCurrentClip(), true);
        t->shader->setInt(texture.first, i);
    }

    t->shader->setMat4("model", transform->getModel());
    t->shader->setMat4("view", coordTransform.view);
    t->shader->setMat4("projection", coordTransform.projection);
    t->shader->setFloat("transparency", t->transparency);
    if (tRigidbody) {
        t->shader->setInt("hasTRigidbody", 1);
        glm::vec2 pos = transform->position;
        t->shader->setVec2("point1", tRigidbody->shape->leftPoint + pos);
        t->shader->setVec2("point2", tRigidbody->shape->rightPoint + pos);
    } else {
        t->shader->setInt("hasTRigidbody", 0);
    }
    glBindVertexArray(t->mainTexture->VAO);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

    if (t->afterRenderFunc)
        t->afterRenderFunc();
     glActiveTexture(GL_TEXTURE0);
     glBindTexture(GL_TEXTURE_2D, 0);
     glBindVertexArray(0);
}

void RenderSystem::render(Model *model) {
    for (auto &gameObject : *model->entities) {

        if (model->beforeRenderFunc)
            model->beforeRenderFunc();
        auto shader = model->shader;
        auto transform = gameObject->getComponent<Transform>();
        auto modelMatrix = transform->getModel();

        shader->use();
        shader->setMat4("model", modelMatrix);
        shader->setMat4("view", coordTransform.view);
        shader->setMat4("projection", coordTransform.projection);
        shader->setVec3("viewPos", camera2D->transform->position); // should be 3D

        for (auto &mesh : model->meshes) {
            // bind appropriate textures
            unsigned int diffuseNr = 1;
            unsigned int specularNr = 1;
            unsigned int normalNr = 1;
            unsigned int heightNr = 1;
            for (unsigned int i = 0; i < mesh.textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i); // active proper texture unit before binding
                // retrieve texture number (the N in diffuse_textureN)
                std::string number;
                std::string name = mesh.textures[i].type;
                if (name == "texture_diffuse")
                    number = std::to_string(diffuseNr++);
                else if (name == "texture_specular")
                    number = std::to_string(specularNr++); // transfer unsigned int to stream
                else if (name == "texture_normal")
                    number = std::to_string(normalNr++); // transfer unsigned int to stream
                else if (name == "texture_height")
                    number = std::to_string(heightNr++); // transfer unsigned int to stream

                // now set the sampler to the correct texture unit
                glUniform1i(glGetUniformLocation(model->shader->ID, (name + number).c_str()), i);
                // and finally bind the texture
                glBindTexture(GL_TEXTURE_2D, mesh.textures[i].id);
            }
            //
            // draw mesh
            glBindVertexArray(mesh.VAO);
            glDrawElements(GL_TRIANGLES, mesh.indices.size(), GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);

            // always good practice to set everything back to defaults once configured.
            glActiveTexture(GL_TEXTURE0);
            if (model->afterRenderFunc)
                model->afterRenderFunc();
        }
    }
}

void RenderSystem::genFramebuffer(GLuint &fb, GLuint &ft, GLuint &ds) {
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
void RenderSystem::initSceneFrambufferData() {
    GLfloat vertices[] = {
        1.0, 1.0, 1.0f, 1.0f,   // top right
        1.0, -1.0, 1.0f, 0.0f,   // bottom right
        -1.0, -1.0, 0.0f, 0.0f,   // bottom left
        -1.0, 1.0, 0.0f, 1.0f    // top left 
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
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void*)(2 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);
}

void RenderSystem::combineTheFramebuffersToFramebuffer(const GLuint& framebufferTarget) {
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
} // namespace wlEngine
