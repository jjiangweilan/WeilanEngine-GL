#pragma once
#include "System.hpp"

#include "Global/Settings.hpp"

#include <glad/glad.h>
#include <SDL.h>
#include <string>
#include <glm/glm.hpp>
#include <stb_image.hpp>
//NOTE: The G-Syn is limiting the frame rate to 60!
namespace WeilanEngine
{
namespace Graphics {
		class Shader;
    class RenderContext;
    class Mesh;
}
class Model;
class Sprite;

class Text;
class Camera;
class RenderContext;

class VolumetricLight;
namespace FramebuffersIndex
{
enum Index
{
  Main,
  VolumetricLight
};
}
class RenderSystem : public System
{
public:
  static RenderSystem *Get();
  static void init();
  ~RenderSystem();

  void render();

  void update() override;

  void setViewPort(int x, int y, int width, int height);

  SDL_GLContext *getSDL_GLContext() { return &glContext; };
  SDL_Window *getSDL_Window() { return window; };
  glm::vec2 getWindowSize() { return {windowWidth, windowHeight}; };
  glm::vec2 GetSceneSize() { return {sceneWidth, sceneHeight}; };

  void SetOutputRenderNode(RenderContext* node);

private:
#if SETTINGS_ENGINEMODE
  void renderGameEditor(unsigned int& sceneTexId);
#endif
#ifdef DEBUG
  void debugRender();
  Graphics::Shader *physicsDebugDrawShader;
  GLuint physicsDebugVAO;
  GLuint physicsDebugVBO;
#endif

  static RenderSystem *renderSystem;
  int windowWidth = 1760;
  int windowHeight = 860;
  int sceneWidth = 1280;
  int sceneHeight = 720;
  const int topMenuHeight = 20;

  /**
     * @brief the first framebuffer is the main scene, the other are used for post-processing
     * 
     */
  std::vector<GLuint> framebuffers;
  std::vector<GLuint> framebufferTextures;
  std::vector<GLuint> depthAndStencilTextures;
  GLuint sceneVAO;
  GLuint sceneVBO;
  GLuint sceneEBO;

  RenderSystem();

  void SDLInit();
  void ImGuiInit();

  void renderGame();
  void render(Sprite *);
  void render(Model *);
  void render(Text *);
  void render(VolumetricLight *);

  void Render(RenderContext*, const bool& loop = false) ;
  void RenderInputSources(RenderContext*, const bool& loop = false);
  void RenderFromScene(RenderContext*) ;
  void RenderModel(RenderContext*) ;
  void RenderToFramebuffer(const Graphics::Mesh*);

  void ResetDrawFlags(RenderContext*);

  SDL_GLContext glContext;
  SDL_Window *window;
  RenderContext *m_outputNode = nullptr;

  glm::mat4 projection;

  int windowResizeCallback(void *data, SDL_Event *event);
  static int windowResizeCallbackWrapper(void *data, SDL_Event *event);

  void PostInit() override;

  friend class PhysicsDebugDraw;
  friend class GameEditor;
  friend class EngineManager;

private:
  EngineManager* m_engine;

  // used to store information for each frame update
  glm::mat4 m_projMatrix;
  glm::mat4 m_viewMatrix;

  void genFramebuffer(GLuint &fb, GLuint &ft, GLuint &ds);
  void initSceneFrambufferData();
};
} // namespace WeilanEngine
