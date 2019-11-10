#pragma once
#include "System.hpp"

#include "../Settings.hpp"

#include <glad/glad.h>
#include <SDL.h>
#include <string>
#include <glm/glm.hpp>
#include <stb_image.hpp>
//NOTE: The G-Syn is limiting the frame rate to 60!
namespace wlEngine
{
namespace Graphics {
		class Shader;
}
class Model;
class Sprite;

class Text;
class GameEditor;
class Camera;

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
  static RenderSystem *get();
  static void init();
  ~RenderSystem();

  void render();

  void update() override;

  void setViewPort(int x, int y, int width, int height);

  SDL_GLContext *getSDL_GLContext() { return &glContext; };
  SDL_Window *getSDL_Window() { return window; };
  glm::vec2 getWindowSize() { return {windowWidth, windowHeight}; };
  glm::vec2 getSceneSize() { return {sceneWidth, sceneHeight}; };
  GameEditor* getGameEditor() { return gameEditor; };

private:
#if SETTINGS_ENGINEMODE
  unsigned int sceneTexture;
  unsigned int sceneFramebuffer;
  unsigned int depthAndStencilTexture;
  void renderGameEditor();
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

  const int FramebufferSize;
  /**
     * @brief the first framebuffer is the main scene, the other are used for post-processing
     * 
     */
  std::vector<GLuint> framebuffers;
  std::vector<GLuint> framebufferTextures;
  std::vector<GLuint> depthAndStencilTextures;
  Graphics::Shader *sceneShader;
  GLuint sceneVAO;
  GLuint sceneVBO;
  GLuint sceneEBO;

  RenderSystem();
  void updateFrameSettings();

  void SDLInit();
  void ImGuiInit();

  void renderGame();
  void render(Sprite *);
  void render(Model *);
  void render(Text *);
  void render(VolumetricLight *);

  SDL_GLContext glContext;
  SDL_Window *window;
  GameEditor *gameEditor;
  Camera *camera = nullptr;

  glm::mat4 projection;

  int windowResizeCallback(void *data, SDL_Event *event);
  static int windowResizeCallbackWrapper(void *data, SDL_Event *event);

  void postInit() override;

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
  void combineTheFramebuffersToFramebuffer(const GLuint &framebufferTarget);

  /* Uniform Buffers */
  GLuint m_projectionUBO;

  void buildInResourceInit();
};
} // namespace wlEngine
