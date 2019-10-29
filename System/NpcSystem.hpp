#pragma once

#include "System.hpp"

namespace wlEngine{
    class NpcController;
    class NpcSystem : public System {
    public:
        static NpcSystem* get() {return system;}
        static void init();
        void update() override;
    private:
        NpcSystem();
        static NpcSystem* system;

        void move(NpcController* npcController);
        void trade(NpcController* npcController);
    };
}