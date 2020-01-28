#pragma once
#include "GameObject/Scene.hpp"

#include <SDL_events.h>
#include <string>
#include <json.hpp>
#include <functional>
namespace WeilanEngine
{
class Entity;
class Sprite;
class Animation;
class TRigidbody;
namespace Graphics
{
class ShaderParameter;
class Material;
} // namespace Graphics
struct GameObjectPack
{
	Entity *child = nullptr;
	Entity *parent = nullptr;
	bool dropped = false;
};
class GameEditor
{
public:
	static GameEditor *get();
	~GameEditor();

	/**
			 * @brief called by UISystem
			 */
	void render(void **data);

	void loadScene(const std::string &);

	/**
			 * @brief this function calculate the mouse click on scene (bottom-left being (0,0))
			 *
			 * @param x
			 * @param y
			 * @param Button right or left click
			 *
			 * @return
			 */
	bool mousePressingOnScene(int &x, int &y, bool world = false, int mouse = 0);

	bool isGameSceneFocused() const;

private:
	static GameEditor *instance;
	GameEditor();
	/********************** Data ***************************/
	Entity *selectedGameObject;
	TRigidbody *selectedTRigidbody;
	bool editVertex;
	bool editLine;
	Scene *scene;
	std::function<void()> helperWindowFunc;
	float gameplayWindowOffsetX;
	float gameplayWindowOffsetY;
	bool m_isGameSceneFocused;

	void setSelectedGameObject(Entity *newOne);

	void showGameObjectInfo();
	void showAllGameObjects();
	void pushGameObject(std::set<Entity *>::iterator iter, const std::set<Entity *> *gameObjects);
	/**
			 * @brief draw to change the scene hierachy
			 *
			 * @param parent
			 */
	void dragDropGameObject();
	GameObjectPack goPack;

	void ShowMenu();

	void showComponent(Entity *, Component *, const std::string &name, std::function<void(Entity *)>);
	void showTRigidbodyInfo(Entity *);
	void showAnimationInfo(Entity *);
	void showTransformInfo(Entity *);
	void showSpriteInfo(Entity *);
	void showVolumetricLightInfo(Entity *);
	void showModelInfo(Entity *);
	void showRenderNodeInfo(Entity *);
	void ShowMaterialInfo(Entity *, const Graphics::Material *);
	void ShowShaderParameterInfo(Graphics::ShaderParameter *);

	void showGameWindow(void **data);

	void showResourceWindow();
	void ShowResourceInDirectory(const std::string &resourceDir);
	void dropSprite(Entity *parent);
	void pickObject();

	/**
			 * @brief we can't remove game object when they are being iterating, this function is called
			 * after all the rendering and operation
			 */
	void removeGameObjects();
	std::set<Entity *> objectToRemove;
	void removeComponent(Entity *go, Component *, const std::string &name);
	void removeComponents();
	struct ComponentRemovePack
	{
		Entity *go;
		Component *component;
		std::string name;
	};
	std::vector<ComponentRemovePack> componentToRemove;

	void dragSprite();

	void setHelperWindowFunc(std::function<void()> f);
	void makeDropDown(const char *selections[], const char **&selected, const int &selectionsSize, const int &widgetId);
	void createTRigidbody();
	void createVolumetricLight();
	void createMaterial();

	template <typename... Args>
	Json createComponentJson(const std::string &name, Args &&...);
	template <typename Arg>
	void createComponentJsonHelper(Json &params, int index, Arg &&);
	template <typename T, typename... Args>
	void createComponentJsonHelper(Json &params, int index, T &&, Args &&...);
};

template <typename... Args>
Json GameEditor::createComponentJson(const std::string &name, Args &&... args)
{
	Json json;
	json["name"] = name;
	json["params"] = Json::array();
	const std::size_t n = sizeof...(Args);
	auto &params = json["params"];
	createComponentJsonHelper(params, 0, args...);
	return json;
}

template <typename Arg>
void GameEditor::createComponentJsonHelper(Json &params, int index, Arg &&arg)
{
	params[index] = arg;
}

template <typename T, typename... Args>
void GameEditor::createComponentJsonHelper(Json &params, int index, T &&arg, Args &&... args)
{
	params[index] = arg;
	createComponentJsonHelper(params, index + 1, args...);
}
} // namespace WeilanEngine
