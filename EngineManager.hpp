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

            
            
            ~EngineManager();


            Scene* getCurrentScene() {return currentScene;};

            void addGameplaySystem(System*);

            void start();
            const unsigned long long& getFrameCount() const;
        private:
            void loop();
			/**
			* @brief initialize anything that needs to be inited before game loop
			*/
			void postInitialization();
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
