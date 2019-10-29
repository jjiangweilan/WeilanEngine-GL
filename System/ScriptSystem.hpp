#pragma once
#include "System.hpp"
namespace wlEngine {
    class ScriptSystem : public System {
	public:
        void update() override;
        void postInit();

		static void init();
		static ScriptSystem* get() { return system; };
	private:
		static ScriptSystem* system;
		ScriptSystem();
    };
}
