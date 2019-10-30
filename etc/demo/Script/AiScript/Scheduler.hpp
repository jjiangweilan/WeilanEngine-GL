#pragma once
#include <wlEngine/wlEngine.hpp>

using namespace wlEngine;

namespace AiScript {
class Scheduler : public Script {
    SCRIPT_DECLARATION(Script, Scheduler, 8);
    std::function<bool()> condition;
    std::function<void()> event;
    Scheduler(Entity *, const std::function<bool()>& condition, const std::function<void()>& event);
    void update() override;

  private:
    Scheduler(Entity *);
};
}; // namespace AiScript