#pragma once
#include "Component.hpp"
#include "Component/Clip.hpp"

#include <map>
#include <string>
#include <vector>
#include <utility>

namespace KuangyeEngine
{
	namespace Graphics {
		class Texture;
}

struct OneAnimationInfo
{
  std::string name;
  Graphics::Texture *atlas;
  std::vector<Frame> frames;
};
using FilePath = std::string;

class Animation : public Component
{
  COMPONENT_DECLARATION(Component, Animation, 100);
  COMPONENT_EDITABLE_DEC();

public:
  Animation(Entity *go);
  Animation(Entity *go, void **args);

  /**
     * @brief 
     * 
     * @param recursive 
     * @param timed if the animation is timed, it will only play the animation if the deltatime is not zero
     */
  void playAnimation(const std::string &, bool recursive = true, bool timed = true);

  /* Animation loading **************************************/
  void addAnimationFromAseprite(const FilePath &animationPath,
                                const FilePath &texturePath);

  Rect *getCurrentClip();
  std::string getCurrentClipName();
  void pause();
  bool isPlaying(const std::string &);
  bool hasEnded();
  int getCurrentFrame();

private:
  OneAnimationInfo *currentAnimation;
  std::map<std::string, OneAnimationInfo> animations;

  int currentFrame = 0;
  float timeStamp = 0;
  bool recursive;
  bool animationHasEnded;

  friend class AnimationSystem;
  friend class GameEditor;
};
} // namespace KuangyeEngine
