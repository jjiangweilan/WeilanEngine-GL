#include "Scheduler.hpp"

namespace AiScript {
    SCRIPT_DEFINATION(Script, Scheduler, 8);

	Scheduler::Scheduler(Entity* entity) : Script(entity) {}
    Scheduler::Scheduler(Entity* entity, const std::function<bool()>& condition, const std::function<void()>& event) : Script(entity), condition(condition), event(event){};

    void Scheduler::update() {
        if (condition()) event();
    }
};