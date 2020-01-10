#pragma once
#include "System.hpp"

namespace KuangyeEngine
{
class Animation;
class AnimationSystem : public System
{
    SYSTEM_DECLARATION(AnimationSystem);

    void updateClip(Animation *);
};
} // namespace KuangyeEngine
