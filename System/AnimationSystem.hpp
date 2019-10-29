#pragma once
#include "System.hpp"

namespace wlEngine {
    class Animation;
    class AnimationSystem : public System{
        SYSTEM_DECLARATION(AnimationSystem);

        void updateClip(Animation*);
    };
}
