#ifndef ENGINE_MANAGER_H
#define ENGINE_MANAGER_H

#include "GameObject/Entity.hpp"
#include "GameObject/Scene.hpp"
#include "Time.hpp"

namespace wlEngine {
    class System;
    class EngineManager {
        public:
            static EngineManager* getwlEngine();

            void setScene(Scene*);

            
            void loop();
            
            ~EngineManager();


            Scene* getCurrentScene() {return currentScene;};

            void addGameplaySystem(System*);
			/**
			* @brief initialize anything that needs to be inited before game loop
			*/
			void postInitialization();
        private:
            bool quit;
            EngineManager();
            static EngineManager* engine;

            Scene* currentScene; 

            void update();

            void initializeSystems();
            void initializeManagers();

            std::vector<System*> gameplaySystem;

            void systemUpdate();
            void SDLinit();
    };

    } // namespace wlEngine

#endif
