#pragma once
#include "Entity.hpp"
#include "GameEditor/SceneData.hpp"

#include <json.hpp>
#include <set>
#include <glm/glm.hpp>
#include <SDL.h>
namespace WeilanEngine {
    class Scene {
        public:
            Scene();
            ~Scene();
            void update();
            void SetMainCamera(Entity* camera);
			void loadScene(const std::string& path);
            void reloadScene();
            Entity* getCamera() {return camera;};
            Entity* findGameObjectByName(const std::string& name);
            const std::set<Entity*>* getSceneGraph(){return &sceneGraph;};

            Entity* CreateGameObject(const std::string& name, Entity* parent = nullptr);
			void addComponent(Entity*, const Json& json);

            /**
             * @brief find the gameobject near mouse x, y
             *
             * @param mouseX
             * @param mouseY
             *
             * @return the gameobject near the position x,y in the scene window
             */
            Entity* findGameObjectNear(const int& mouseX, const int& mouseY);
            /**
             * @brief destroy allocated game object
             *
             * @param GameObject
             */
            void destroyGameObject(Entity*);

            void addGameObject(Entity*);
            SceneData sceneData;
            std::string filePath;
        private:
			std::set<Entity*> allocatedGameObjects;
			std::set<Entity*> sceneGraph;
            std::vector<Entity*> entityToBeDeleted;

            Entity* camera;

            //resonsible for converting z position to x and y
            void render();
            void clearScene();
            void destoryGameObjectReal(Entity*); // called at the end of the frame;
			void destroyGameObjectHelper(Entity*); // delte gamobject without move from parent object to prevent iterator change when looping
            void endFrame();
			Entity* createGameObjectByJson(const Json&, Entity* parent);
            
            /**
             * @brief load from child to parent
             *
             * @param json
             * @param id
             * @param loadedGameObjects
             *
             * @return 
             */
            Entity* loadGameObjectFromFile(const Json& json, const std::string& id, const Json& jsonFile, std::map<std::string, Entity*>& loadedGameObjects);
            Entity* findGameObjectNearHelper(std::set<Entity*>::iterator iter, std::set<Entity*>* set, const int&, const int&);
            Entity* findGameObjectByNameHelper(std::set<Entity*>::iterator iter, std::set<Entity*>* set, const std::string& name);

            FixedArrayAllocator<Entity, 3000> gameObjectAllocator;

            friend FixedArrayAllocator<Entity, 3000>;
            friend class EngineManager;
            friend class Entity;
            friend class GameEditor;

    };
}
