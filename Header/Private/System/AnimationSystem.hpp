#pragma once
#include "System.hpp"

namespace WeilanEngine
{
class Animation;
class AnimationSystem : public System
{
    SYSTEM_DECLARATION(AnimationSystem);

    void updateClip(Animation *);
};
} // namespace WeilanEngine
