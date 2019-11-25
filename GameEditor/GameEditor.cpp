
#include "GameEditor.hpp"

#include "../third_party/imgui/imgui.h"
#include "../EngineManager.hpp"
#include "../Settings.hpp"

#include "../Manager/ResourceManager.hpp"

#include "../Component/Transform.hpp"
#include "../Component/Animation.hpp"
#include "../Component/Sprite.hpp"
#include "../Component/TRigidbody.hpp"
#include "../Component/VolumetricLight.hpp"
#include "../Component/Camera2D.hpp"
#include "../Component/Camera3D.hpp"
#include "../Component/Model.hpp"

#include "../System/RenderSystem.hpp"
#include "../System/InputSystem.hpp"

#include "Graphics/Model.hpp"

#include "../Utility/Utility.hpp"

#include <fstream>
#include <sstream>
#include <dirent.h>
namespace wlEngine
{
GameEditor::GameEditor() : selectedGameObject(nullptr), selectedTRigidbody(nullptr), editVertex(false), editLine(false), m_isGameSceneFocused(true), scene(nullptr)
{
	
}

GameEditor::~GameEditor()
{
}

void GameEditor::render(void **data)
{
    scene = EngineManager::getwlEngine()->getCurrentScene();
    ImGui::GetIO().ConfigWindowsMoveFromTitleBarOnly = true;

    if (Input::getKeyStatus(SDL_SCANCODE_ESCAPE))
    {
        selectedGameObject = nullptr;
    }

    showGameWindow(data);
    showMenu();
    showAllGameObjects();
    showResourceWindow();

    removeGameObjects();
    removeComponents();
    dragDropGameObject();

    pickObject();

    ImGui::SetNextWindowPos({160, 415}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({275, 350}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Helper Window");
    if (helperWindowFunc)
        helperWindowFunc();
    ImGui::End();
    //update camera
    auto camera = scene->getCamera();
    auto &goJson = scene->sceneData.getData(camera);
    Json *transformJsonPtr = Utility::findComponentWithName(goJson, "Transform");
    if (transformJsonPtr)
    {
        Json &transformJson = *transformJsonPtr;
        auto pos = camera->GetComponent<Transform>()->position;
        transformJson["params"][0] = pos.x;
        transformJson["params"][1] = pos.y;
        transformJson["params"][2] = pos.z;
    }
}

void GameEditor::pickObject()
{
    if (Settings::gameDimension == Settings::GameDimension::Two) return; 
    static int x,y;
    static bool pressing = false;
    if (Input::isMouseClicked(MouseButton::Left))
    {
        glm::vec3 origin;
        glm::vec3 end;
        Utility::getRayFromScreenToWorld(x, y, origin, end);
        float min = FLT_MAX;
        for (auto model : Model::collection)
        {
            auto gModel = model->getModel();
            auto aabb = gModel->getAABB();
            float distance;
            bool interact = Utility::TestRayOBBIntersection(origin, end - origin,
                                                            aabb.min, aabb.max,
                                                            model->entity->GetComponent<Transform>()->getModel(),
                                                            distance);
            if (interact && distance < min) {
                min = distance;
                setSelectedGameObject(model->entity);
            }
        }
    }

    if (pressing && selectedGameObject) 
    {
        int relX, relY;
        mousePressingOnScene(relX, relY);
        relX = relX - x;
        relY = relY - y;
        auto camera = EngineManager::getwlEngine()->getCurrentScene()->getCamera()->GetComponent<Camera3D>();
        auto front = camera->front;
        auto right = camera->right;

        front = {front.x, 0.0, front.z};
        right = {right.x, 0.0, right.z};

        auto transform =selectedGameObject->GetComponent<Transform>();
		
        transform->moveBy(right *= (float)relX * Time::deltaTime * 2.0f);
        if (Input::getKeyStatus(SDL_SCANCODE_LCTRL)) 
        {   
            transform->moveBy(0,relY * Time::deltaTime * 2.0f,0);
        }
        else
        {
            transform->moveBy(front * (float)relY *  Time::deltaTime  * 2.0f);
        }
		//Input::enableRelativeMouse(true);
    }
	else
	{
		//Input::enableRelativeMouse(false);
	}

	pressing = mousePressingOnScene(x, y);
}

void GameEditor::removeComponents()
{
    for (size_t i = 0; i < componentToRemove.size(); i++)
    {
        removeComponent(componentToRemove[i].go, componentToRemove[i].component, componentToRemove[i].name);
    }
    componentToRemove.clear();
}

void GameEditor::removeGameObjects()
{
    for (auto iter : objectToRemove)
    {
        scene->destroyGameObject(iter);
        scene->sceneData.destroyGameObject(iter);
        if (iter == selectedGameObject)
            setSelectedGameObject(nullptr);
    }
    objectToRemove.clear();
}

void GameEditor::showGameWindow(void **data)
{
    auto &style = ImGui::GetStyle();
    auto windowPadding = style.WindowPadding;
    style.WindowPadding = {0, 0};
    std::string windowName;
    if (Settings::engineMode == Settings::EngineMode::Gameplay)
        windowName = "Game: GamePlay";
    else
        windowName = "Game: Editor";
    if (selectedTRigidbody && editVertex)
        windowName += " Editing Vertex";
    if (selectedTRigidbody && editLine)
        windowName += " Editing Render Line";
    windowName += "###GameScene";
    ImGui::SetNextWindowPos({450, 30}, ImGuiCond_FirstUseEver);
    ImGui::Begin(windowName.data(), nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_AlwaysAutoResize);
    m_isGameSceneFocused = ImGui::IsWindowFocused();
    gameplayWindowOffsetX = ImGui::GetWindowPos().x;
    gameplayWindowOffsetY = ImGui::GetWindowPos().y + 20;
    float sceneWidth = *(int *)(data[1]);
    float sceneHeight = *(int *)(data[2]);
    ImGui::Image((void *)*(unsigned int *)(data[0]), {sceneWidth, sceneHeight}, {0, 1}, {1, 0}); //one commnet from imgui.cpp: my_void_ptr = (void*)(intptr_t)my_tex;                  // cast a GLuint into a void* (we don't take its address! we literally store the value inside the pointer)
    dragSprite();

    ImGui::End();

    style.WindowPadding = windowPadding;
}

void GameEditor::showAllGameObjects()
{
    ImGui::SetNextWindowPos({5, 415}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({150, 350}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Scene Graph", nullptr, ImGuiWindowFlags_None);
    if (ImGui::BeginPopupContextItem())
    {
        if (ImGui::MenuItem("Add Game Object"))
        {
            Json go_json;
            go_json["name"] = "New GameObject";
            go_json["components"] = Json::array();
            go_json["children"] = Json::array();
            Json transformParams = Json::array({0, 0, 0});

            Json transformJson;

            transformJson["name"] = "Transform";
            transformJson["params"] = transformParams;

            go_json["components"].push_back(transformJson);

            scene->createGameObjectByJson(go_json, nullptr);
        }
        ImGui::EndPopup();
    }
    auto gameObjects = scene->getSceneGraph();
    dropSprite(nullptr);
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("GameObject"))
        {
            goPack.child = *static_cast<Entity **>(payload->Data);
            goPack.parent = nullptr;
            goPack.dropped = true;
        }
        ImGui::EndDragDropTarget();
    }
    pushGameObject(gameObjects->begin(), gameObjects);
    ImGui::End();
    showGameObjectInfo();
}

void GameEditor::pushGameObject(std::set<Entity *>::iterator iter, const std::set<Entity *> *gameObjects)
{
    if (iter != gameObjects->end())
    {
        bool open = ImGui::TreeNodeEx((*iter)->name.c_str(), ImGuiTreeNodeFlags_OpenOnArrow);

        //dragNdrop to change scene hierachy
        if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
        {
            ImGui::SetDragDropPayload("GameObject", &*iter, sizeof(Entity *));
            ImGui::Text("%s", (*iter)->name.c_str());
            ImGui::EndDragDropSource();
        }
        if (ImGui::BeginDragDropTarget())
        {
            if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("GameObject"))
            {
                goPack.child = *static_cast<Entity **>(payload->Data);
                goPack.parent = *iter;
                goPack.dropped = true;
            }
            ImGui::EndDragDropTarget();
        }

        dropSprite(*iter);

        //select game object
        if (ImGui::IsItemClicked() && (ImGui::GetMousePos().x - ImGui::GetItemRectMin().x) > ImGui::GetTreeNodeToLabelSpacing())
        {
            setSelectedGameObject(*iter);
        }

        if (open)
        {
            pushGameObject((*iter)->children.begin(), &(*iter)->children);
            ImGui::TreePop();
        }
        pushGameObject(++iter, gameObjects);
    }
}

void GameEditor::showComponent(Entity *go, Component *c, const std::string &name, std::function<void(Entity *)> f)
{

    bool open = ImGui::TreeNodeEx(name.data());
    if (ImGui::BeginPopupContextItem())
    {
        //remove component
        if (ImGui::MenuItem("Remove"))
        {
            componentToRemove.push_back(ComponentRemovePack{go, c, name});
        }
        ImGui::EndPopup();
    }
    if (open)
    {
        if (f)
            f(go);
        ImGui::TreePop();
    }
}

void GameEditor::showGameObjectInfo()
{
    ImGui::SetNextWindowPos({5, 200}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({430, 200}, ImGuiCond_FirstUseEver);
    ImGui::Begin("GameObject");
    if (selectedGameObject)
    {
        //gameobject little menu
        if (ImGui::BeginPopupContextItem())
        {
            if (ImGui::TreeNodeEx("Add Script"))
            {
                for (auto f : *Component::componentFactoryList)
                {
                    auto componentIter = Component::getComponentIdToName()->find(f.first);
                    if (componentIter != Component::getComponentIdToName()->end())
                    {
                        std::string componentName = (*Component::getComponentIdToName()).at(f.first);
                        if (ImGui::MenuItem(componentName.data()))
                        {
                            Json j;
                            j["name"] = componentIter->second;
                            j["params"] = Json::array();
                            Json components = Json::array();
                            components.push_back(j);
                            scene->addComponent(selectedGameObject, components);
                            scene->sceneData.addComponent(selectedGameObject, components);
                        }
                    }
                }
                ImGui::TreePop();
            }
            //add TRigidbody
            if (ImGui::Button("Add TRigidbody"))
            {
                setHelperWindowFunc([this]() { this->createTRigidbody(); });
            }
            if (ImGui::Button("Add VolumetricLight"))
            {
                setHelperWindowFunc([this]() { this->createVolumetricLight(); });
            }
            //remove gameobject
            if (ImGui::Button("Remove"))
            {
                objectToRemove.insert(selectedGameObject);
            }
            ImGui::EndPopup();
        }

        Entity *go = selectedGameObject;
        char name[512];
        strcpy(name, go->name.data());
        if (ImGui::InputText("Object Name", name, 512))
        {
            go->name = name;
            scene->sceneData.getData(go)["name"] = name;
        }
        for (auto &c : go->components)
        {
            if (c->getId() == Transform::componentId)
                showComponent(go, c.get(), "Transform", std::bind(&GameEditor::showTransformInfo, this, std::placeholders::_1));
            else if (c->getId() == TRigidbody::componentId)
                showComponent(go, c.get(), "TRigidbody", std::bind(&GameEditor::showTRigidbodyInfo, this, std::placeholders::_1));
            else if (c->getId() == VolumetricLight::componentId)
            {
                showComponent(go, c.get(), "Volumetric Light", std::bind(&GameEditor::showVolumetricLightInfo, this, std::placeholders::_1));
            }
            else if (c->getId() == Model::componentId)
            {
                showComponent(go, c.get(), "Model", std::bind(&GameEditor::showModelInfo, this, std::placeholders::_1));
            }
            //else if (c->getId() == Sprite::componentId)showComponent(go, c.get(), "Sprite", std::bind(&GameEditor::showSpriteInfo, this, std::placeholders::_1));
            //else if (c->getId() == Animation::componentId)showComponent(go, c.get(), "Animation", std::bind(&GameEditor::showAnimationInfo, this, std::placeholders::_1));
            else
            {
                std::size_t t = c->getId();
                auto iter = Component::getComponentIdToName()->find(t);
                if (iter != Component::getComponentIdToName()->end())
                {
                    showComponent(go, c.get(), iter->second.data(), nullptr);
                    //ImGui::TreeNodeEx(iter->second.data(), ImGuiTreeNodeFlags_Leaf);
                    //ImGui::TreePop();
                }
            }
        }
    }
    ImGui::End();
}
void GameEditor::showModelInfo(Entity *entity)
{
    auto model = entity->GetComponent<Model>();
    auto gModel = model->getModel();
    for (auto& mesh : *gModel->GetMeshes())
    {
        auto params = mesh.GetMaterial()->GetParameters();
        for (auto& param : params->m_parameters)
        {
			auto type = param.second->GetType();
			switch (type)
			{
			case Graphics::ParameterType::Float:
				float* val = static_cast<float*>(param.second->Get());
				ImGui::InputFloat(param.first.data(), val, 0.05, 0.1, 3);
				break;
			}
        }
    }
}

void GameEditor::showTRigidbodyInfo(Entity *entity)
{
    //params  0. type string 1. shape string 2. vertices or radius int[0]...int[n]
    auto trb = entity->GetComponent<TRigidbody>();

    int sensor = trb->sensor;
    bool changed = ImGui::InputInt("sensor", &sensor);
    if (changed)
    {
        trb->sensor = sensor;
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][5] = trb->sensor;
        }
    }

    int newCategory = trb->category;
    changed = ImGui::InputInt("category", &newCategory);
    if (newCategory < 0 || newCategory > 0xFFFF)
        assert(0 && "category is out of range");
    if (changed)
    {
        trb->category = newCategory;
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][4][0] = newCategory;
        }
    }

    int newMask = trb->mask;
    changed = ImGui::InputInt("mask", &newMask);
    if (newMask < 0 || newMask > 0xFFFF)
        assert(0 && "make is out of range");
    if (changed)
    {
        trb->mask = newMask;
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][4][1] = newMask;
        }
    }

    if (ImGui::MenuItem("Edit Vertex"))
    {
        if (selectedTRigidbody && editVertex)
        {
            editVertex = false;
            selectedTRigidbody = nullptr;
        }
        else
        {
            editVertex = true;
            editLine = false;
            selectedTRigidbody = entity->GetComponent<TRigidbody>();
        }
    }
    // vertex editing
    if (trb->shape->getShapeType() == ShapeType::Polygon)
    {
        PolygonShape *pShape = static_cast<PolygonShape *>(trb->shape);
        for (int i = 0; i < pShape->points.size(); i++)
        {
            int ver[2] = {pShape->points[i].x, pShape->points[i].y};
            bool set = ImGui::InputInt2(std::string("vertex " + std::to_string(i)).c_str(), ver);
            if (set)
            {
                pShape->setVertexPos(i, glm::vec2(ver[0], ver[1]));
                auto &goJson = scene->sceneData.getData(trb->entity);
                Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
                if (TRigidbodyJsonPtr)
                {
                    Json &trbJson = *TRigidbodyJsonPtr;
                    if (TRigidbodyJsonPtr)
                    {
                        Json &trbJson = *TRigidbodyJsonPtr;
                        trbJson["params"][2][i] = ver;
                    }
                }
            }
        }
    }
    else if (trb->shape->getShapeType() == ShapeType::Line)
    {
        LineShape *lShape = static_cast<LineShape *>(trb->shape);
        for (int i = 0; i < lShape->points.size(); i++)
        {
            int ver[2] = {lShape->points[i].x, lShape->points[i].y};
            bool set = ImGui::InputInt2(std::string("vertex " + std::to_string(i)).c_str(), ver);
            if (set)
            {
                lShape->setVertexPos(i, glm::vec2(ver[0], ver[1]));
                auto &goJson = scene->sceneData.getData(trb->entity);
                Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
                if (TRigidbodyJsonPtr)
                {
                    Json &trbJson = *TRigidbodyJsonPtr;
                    trbJson["params"][2][i] = ver;
                }
            }
        }
    }

    if (ImGui::MenuItem("Edit Render Line"))
    {
        if (selectedTRigidbody && editLine)
        {
            editLine = false;
            selectedTRigidbody = nullptr;
        }
        else
        {
            editLine = true;
            editVertex = false;
            selectedTRigidbody = entity->GetComponent<TRigidbody>();
        }
    }

    // line editing
    Shape *shape = trb->shape;
    int ver[2] = {shape->leftPoint.x, shape->leftPoint.y};
    bool set = ImGui::InputInt2("leftPoint", ver);
    if (set)
    {
        shape->leftPoint = {ver[0], ver[1]};
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][3][0] = ver;
        }
    }

    ver[0] = shape->rightPoint.x;
    ver[1] = shape->rightPoint.y;
    set = ImGui::InputInt2("rightPoint", ver);
    if (set)
    {
        shape->rightPoint = {ver[0], ver[1]};
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][3][1] = ver;
        }
    }

    if (ImGui::MenuItem("Clear Vertex"))
    {
        trb->clear();
        auto &goJson = scene->sceneData.getData(trb->entity);
        Json *TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
        if (TRigidbodyJsonPtr)
        {
            Json &trbJson = *TRigidbodyJsonPtr;
            trbJson["params"][2] = Json::array();
        }
        else
        {
            std::cout << "this TRigidbody is init from scipt" << std::endl;
        }
    }

    // add vertex to rigidbody and edit render line
    if (selectedTRigidbody)
    {
        int ver[2];
        if (mousePressingOnScene(ver[0], ver[1], true, 1) && ImGui::IsMouseClicked(1))
        {
            auto goTrans = selectedTRigidbody->entity->GetComponent<Transform>();
            ver[0] += (-goTrans->position.x);
            ver[1] += (-goTrans->position.y);
            auto &goJson = scene->sceneData.getData(trb->entity);
            auto TRigidbodyJsonPtr = Utility::findComponentWithName(goJson, "TRigidbody");
            static int editingPoint = 0; // 0 for left, 1 for right

            if (editVertex)
            {
                auto shape = selectedTRigidbody->shape->getShapeType();
                if (shape == ShapeType::Polygon)
                {
                    selectedTRigidbody->pushbackVertex<PolygonShape>({ver[0], ver[1]});
                }
                else if (shape == ShapeType::Line)
                {
                    selectedTRigidbody->pushbackVertex<LineShape>({ver[0], ver[1]});
                }

                if (TRigidbodyJsonPtr)
                {
                    Json &trbJson = *TRigidbodyJsonPtr;
                    trbJson["params"][2].push_back(ver);
                }
            }
            else if (editLine)
            {
                auto shape = selectedTRigidbody->shape;
                if (editingPoint == 0)
                {
                    shape->leftPoint = {ver[0], ver[1]};
                    editingPoint = 1;
                    if (TRigidbodyJsonPtr)
                    {
                        Json &trbJson = *TRigidbodyJsonPtr;
                        trbJson["params"][3][0] = ver;
                    }
                }
                else if (editingPoint == 1)
                {
                    shape->rightPoint = {ver[0], ver[1]};
                    editingPoint = 0;
                    if (TRigidbodyJsonPtr)
                    {
                        Json &trbJson = *TRigidbodyJsonPtr;
                        trbJson["params"][3][1] = ver;
                    }
                }
            }
            if (!TRigidbodyJsonPtr)
            {
                std::cout << "this TRigidbody is init from scipt" << std::endl;
            }
        }
    }
}

void GameEditor::showMenu()
{
    if (ImGui::BeginMainMenuBar())
    {
        if (ImGui::BeginMenu("File"))
        {
            ImGui::MenuItem("New Scene", "CTRL+N");
            if (ImGui::IsItemClicked())
            {
                std::string filePath = "../mainScene";
                while (std::ifstream(filePath))
                {
                    filePath += " New";
                }
                std::ofstream ofs;
                ofs.open(filePath);
                if (ofs.good())
                {
                    Entity *nil = nullptr;
                    ofs << "{\"gameObjects\": {\"1\" : {\"children\": [] , \"components\" : [{\"name\" : \"Camera2D\", \"params\" : []}, {\"name\" : \"Transform\", \"params\" : [0,0,0]}], \"name\" : \"Camera\", \"parent\" :" + Utility::toPointerString(nil) + "}} }";
                }
                ofs.close();
            }
            ImGui::MenuItem("Save", "CTRL+S");
            if (ImGui::IsItemClicked())
            {
                const nlohmann::json &scene_jsonRef = scene->sceneData.data;
                std::ofstream ofs;
                ofs.open(scene_jsonRef["scene_path"].get<std::string>());
                if (ofs.good())
                {
                    ofs << scene_jsonRef.dump();
                }
                ofs.close();
            }
            ImGui::MenuItem("Reload", "CTRL+S");
            if (ImGui::IsItemClicked())
            {
                scene->loadScene(scene->filePath);
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Edit"))
        {
            if (ImGui::MenuItem("Undo", "CTRL+Z"))
            {
            }
            if (ImGui::MenuItem("Redo", "CTRL+Y", false, false))
            {
            } // Disabled item
            ImGui::Separator();
            if (ImGui::MenuItem("Cut", "CTRL+X"))
            {
            }
            if (ImGui::MenuItem("Copy", "CTRL+C"))
            {
            }
            if (ImGui::MenuItem("Paste", "CTRL+V"))
            {
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Tool"))
        {
            if (ImGui::MenuItem("Create Material"))
            {
                setHelperWindowFunc([this]() { this->createMaterial(); });
            }
            ImGui::EndMenu();
        }
        if (ImGui::BeginMenu("Settings"))
        {
            static int dimension = 2;
            if (dimension = 2)
            {
                ImGui::MenuItem("Switch To 3D");
                if (ImGui::IsItemClicked())
                {
                    auto cameraEntity = EngineManager::getwlEngine()->getCurrentScene()->getCamera();
                    Json &j = scene->sceneData.getData(cameraEntity);
                    auto &cameraComponentJson = *Utility::findComponentWithName(j, "Camera2D");
                    //cameraComponentJson["name"] = "Camera3D";
                    cameraEntity->removeComponent<Camera2D>();
                    cameraEntity->AddComponent<Camera3D>();
                }
            }
            else if (dimension = 3)
            {
                ImGui::MenuItem("Switch To 2D");
                if (ImGui::IsItemClicked())
                {
                }
            }
            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }
}

void GameEditor::showAnimationInfo(Entity *go)
{
    auto animation = go->GetComponent<Animation>();
    std::string current_item = animation->getCurrentClipName();
    if (ImGui::BeginCombo("animation", current_item.data()))
    {
        for (auto iter : animation->animations)
        {
            bool isSelected = (current_item == iter.first); // You can store your selection however you want, outside or inside your objects
            if (ImGui::Selectable(iter.first.data(), &isSelected))
            {
                animation->playAnimation(iter.first);
                ImGui::SetItemDefaultFocus(); // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
            }
        }
        ImGui::EndCombo();
    }
    Rect *clip = animation->getCurrentClip();
    ImGui::Text("offset X: %d", clip->x);
    ImGui::Text("offset Y: %d", clip->y);
    ImGui::Text("width: %d", clip->width);
    ImGui::Text("height: %d", clip->height);
}

void GameEditor::showTransformInfo(Entity *go)
{
    auto transform = go->GetComponent<Transform>();
    auto pos = transform->position;

    bool inputX = ImGui::InputFloat("x", &pos.x);
    bool inputY = ImGui::InputFloat("y", &pos.y);
    bool inputZ = ImGui::InputFloat("z", &pos.z);

    if (inputX || inputY || inputZ)
    {
        transform->setPosition(pos);
        Json &json = scene->sceneData.getData(go);
        Json *transform = Utility::findComponentWithName(json, "Transform");
        if (transform)
        {
            auto &t = *transform;
            t["params"][0] = pos.x;
            t["params"][1] = pos.y;
            t["params"][2] = pos.z;
        }
    }

    float scaleX = transform->scale.x;
    float scaleY = transform->scale.y;

    bool edit0 = ImGui::InputFloat("ScaleX", &scaleX, 0.005, 0.05, 3);
    bool edit1 = ImGui::InputFloat("ScaleY", &scaleY, 0.005, 0.05, 3);
    if (edit0 || edit1)
    {
        transform->setScale(scaleX, scaleY, 1.0);
    }

    float rotate = transform->rotationData.degree;
    float axis[3] = {transform->rotationData.axis.x, transform->rotationData.axis.y, transform->rotationData.axis.z};
    edit0 = ImGui::InputFloat("rotate degree", &rotate, 1.0, 20.0, 0);
    edit1 = ImGui::InputFloat3("rotate axis", axis, 0);
    if (edit0 || edit1)
    {
        transform->rotate(glm::vec3{axis[0], axis[1], axis[2]}, rotate);
    }
}
void GameEditor::showResourceWindow()
{
    ImGui::SetNextWindowPos({5, 30}, ImGuiCond_FirstUseEver);
    ImGui::SetNextWindowSize({430, 160}, ImGuiCond_FirstUseEver);
    ImGui::Begin("Resource", nullptr, ImGuiWindowFlags_HorizontalScrollbar);
    showResourceInDirectory("../resource");
    ImGui::End();
}
void GameEditor::showSpriteInfo(Entity *go)
{
    auto sprite = go->GetComponent<Sprite>();
    auto mainTexture = sprite->getMesh()->getTextures()->at(0);
    ImGui::Image((void *)mainTexture->GetId(), {(float)mainTexture->getWidth(), (float)mainTexture->getHeight()}, {0, 1}, {1, 0});
}
void GameEditor::showResourceInDirectory(const std::string &path)
{
    DIR *dir;
    struct dirent *ent;
    /* Open directory stream */
    dir = opendir(path.data());
    if (dir != NULL)
    {
        if (ImGui::TreeNodeEx(path.data()))
        {
            std::vector<std::string> directory;
            std::vector<std::string> files;
            while ((ent = readdir(dir)) != NULL)
            {
                switch (ent->d_type)
                {
                case DT_REG:
                {
                    files.push_back(path + "/" + ent->d_name);
                    break;
                }

                case DT_DIR:
                    if (ent->d_name[0] != '.')
                        directory.push_back(path + "/" + ent->d_name);
                    break;

                default:
                    printf("%s*\n", ent->d_name);
                }
            }

            for (auto &dir : directory)
            {
                showResourceInDirectory(dir);
            }
            for (auto &filePath : files)
            {
                std::size_t dotPos = filePath.find_last_of('.');
                std::string etx = filePath.substr(dotPos + 1, filePath.length());
                std::transform(etx.begin(), etx.end(), etx.begin(), [](unsigned char c) { return std::tolower(c); });
                if (etx == "png" || etx == "jpg")
                {
                    std::string name = filePath.substr(filePath.find_last_of("/") + 1, filePath.length());
                    if (ImGui::TreeNodeEx(name.data()))
                    {
                        auto texture = Graphics::Texture::add(filePath, filePath);
                        ImGui::Image((void *)texture->GetId(), {(float)texture->getWidth(), (float)texture->getHeight()}, {0, 1}, {1, 0});
                        ImGui::TreePop();
                    }
                    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
                    {
                        auto texture = Graphics::Texture::add(filePath, filePath);
                        ImGui::SetDragDropPayload("Sprite", &texture, sizeof(Graphics::Texture *)); // Set payload to carry the index of our item (could be anything)
                        ImGui::Text("%s", filePath.data());
                        ImGui::EndDragDropSource();
                    }
                }
            }
            ImGui::TreePop();
        }

        closedir(dir);
    }
    else
    {
        std::cerr << "Resource Manager path error\n";
    }
}

void GameEditor::dropSprite(Entity *parent)
{
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("Sprite"))
        {
            Graphics::Texture *texture_ptr = *static_cast<Graphics::Texture **>(payload->Data);
            Json go_json;
            auto &sourcePath = texture_ptr->getSourcePath();
            go_json["name"] = sourcePath.substr(sourcePath.find_last_of("/") + 1, sourcePath.size() - 4); // being lazy here, -4 becuase .jpg and .png all have 4 characters
            go_json["components"] = Json::array();
            go_json["children"] = Json::array();
            Json spriteParams = Json::array({sourcePath, "sprite_shader"});
            Json transformParams = Json::array({0, 0, 0});

            Json spriteJson;
            Json transformJson;
            spriteJson["name"] = "Sprite";
            spriteJson["params"] = spriteParams;

            transformJson["name"] = "Transform";
            transformJson["params"] = transformParams;

            go_json["components"].push_back(transformJson);
            go_json["components"].push_back(spriteJson);

            scene->createGameObjectByJson(go_json, parent);
        }
        ImGui::EndDragDropTarget();
    }
}

void GameEditor::dragDropGameObject()
{
    if (goPack.dropped)
    {
        goPack.child->SetParent(goPack.parent);
        goPack.dropped = false;
        scene->sceneData.changeHierachy(goPack.parent, goPack.child);
    }
}

void GameEditor::removeComponent(Entity *go, Component *c, const std::string &name)
{
    go->RemoveComponent(c);
    scene->sceneData.removeComponent(go, name);
}

void GameEditor::dragSprite()
{
    if (Settings::gameDimension == Settings::GameDimension::Three)
        return;
    static Transform *target = nullptr;
    static int lastX, lastY;
    if (ImGui::IsMouseClicked(2))
    {
        ImGui::SetWindowFocus("###GameScene");
    }
    if (ImGui::IsWindowFocused())
    {
        int mouseX, mouseY;
        if (mousePressingOnScene(mouseX, mouseY, true) || ImGui::IsMouseDown(2))
        {
            Entity *go;
            if (ImGui::IsMouseDown(0))
                go = scene->findGameObjectNear(mouseX, mouseY);
            else if (ImGui::IsMouseDown(2))
            {
                go = selectedGameObject;
            }
            if (go && !target)
            {
                target = go->GetComponent<Transform>();
                setSelectedGameObject(go);
                lastX = mouseX;
                lastY = mouseY;
            }
        }
        else
        {
            target = nullptr;
        }
        if (target)
        {
            target->moveBy(mouseX - lastX, mouseY - lastY, 0);
            lastX = mouseX;
            lastY = mouseY;
            scene->sceneData.editTransform(selectedGameObject, target->position.x, target->position.y, target->position.z);
        }
    }
}

GameEditor *GameEditor::instance = nullptr;
GameEditor *GameEditor::get()
{
    if (instance)
        return instance;
    instance = new GameEditor();
    return instance;
}
/********************** Helper ****************************/
bool GameEditor::mousePressingOnScene(int &x, int &y, bool world, int mouse)
{
    if (!scene)
        return false;
    auto sceneSize = RenderSystem::get()->getSceneSize();

    auto mousePos = ImGui::GetMousePos();
    auto cameraPos = scene->getCamera()->GetComponent<Transform>()->position;
    bool onWindow = false;
    if (ImGui::IsMouseDown(mouse))
    { // 0 left, 1 right, 2 mid
        int sceneHeight = RenderSystem::get()->getSceneSize().y;

        x = mousePos.x - gameplayWindowOffsetX;
        y = sceneHeight + gameplayWindowOffsetY - mousePos.y;
        if (x >= 0 && y >= 0 && x <= sceneSize.x && y <= sceneSize.y)
            onWindow = true;
        if (world)
        {
            x += cameraPos.x;
            y += cameraPos.y;
        }
        return onWindow;
    }
    int sceneHeight = RenderSystem::get()->getSceneSize().y;
    x = mousePos.x - gameplayWindowOffsetX;
    y = sceneHeight + gameplayWindowOffsetY - mousePos.y; // 20 is the menu height
    if (world)
    {
        x += cameraPos.x;
        y += cameraPos.y;
    }
    return onWindow;
}

void GameEditor::createTRigidbody()
{
    if (selectedGameObject->GetComponent<TRigidbody>())
    {
        helperWindowFunc = nullptr;
        return;
    }
    const char *bodyTypeSelections[] = {"dynamic", "static"};
    static const char **selectedBodyType = &bodyTypeSelections[0];
    makeDropDown(bodyTypeSelections, selectedBodyType, IM_ARRAYSIZE(bodyTypeSelections), 0);
    const char *shapeSelections[] = {"polygon", "circle", "line"};
    static const char **selectedShape = &shapeSelections[0];
    makeDropDown(shapeSelections, selectedShape, IM_ARRAYSIZE(shapeSelections), 1);

    if (ImGui::Button("Create"))
    {
        auto bodyType = selectedBodyType == &bodyTypeSelections[0] ? BodyType::Dynamic : BodyType::Static;
        Shape *shape;
        if (selectedShape == &shapeSelections[0])
            shape = new PolygonShape({});
        else if (selectedShape == &shapeSelections[1])
            shape = new CircleShape({0, 0}, 0);
        else
            shape = new LineShape({});
        selectedGameObject->AddComponent<TRigidbody>(shape, bodyType);

        //add json information to saved game data
        Json trbJson = createComponentJson("TRigidbody", "static", "polygon", Json::array(), Json::array({{-1, 0}, {1.0}}), Json::array({0.0}), false);
        Json components = Json::array();
        components.push_back(trbJson);
        scene->sceneData.addComponent(selectedGameObject, components);

        //clean up
        delete shape;
        helperWindowFunc = nullptr;
    }
}

void GameEditor::makeDropDown(const char *selections[], const char **&selected, const int &selectionsSize, const int &widgetId)
{
    ImGui::PushID(widgetId);
    if (ImGui::BeginCombo("##combo", *selected))
    {
        for (int n = 0; n < selectionsSize; n++)
        {
            bool is_selected = (selected == &selections[n]);
            if (ImGui::Selectable(selections[n], is_selected))
                selected = &selections[n];
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }
    ImGui::PopID();
}

void GameEditor::createVolumetricLight()
{
    static int total = 0;
    static std::vector<std::string> files;
    if (selectedGameObject->GetComponent<TRigidbody>())
    {
        helperWindowFunc = nullptr;
        return;
    }

    for (int i = 0; i < total; i++)
    {
        char file[256];
        std::strcpy(file, files[i].data());
        ImGui::InputText((std::string("Texture") + std::to_string(i)).data(), file, 256);
        files[i] = file;
    }

    if (ImGui::Button("Add texture"))
    {
        total += 1;
        files.push_back("");
    }
    ImGui::SameLine(0, 5);
    if (ImGui::Button("Remove"))
    {
        total -= 1;
        files.pop_back();
    };

    if (ImGui::Button("Create"))
    {
        auto vl = selectedGameObject->AddComponent<VolumetricLight>();
        for (int i = 0; i < total; i++)
        {
            vl->loadTexture(files[i]);
        }

        //TODO:
        //add json information to saveddata
        //Json trbJson = createComponentJson("VolumetricLight", fi, "polygon", Json::array(), Json::array({{-1, 0}, {1.0}}), Json::array({0.0}), false);
        //Json components = Json::array();
        //components.push_back(trbJson);
        //scene->sceneData.addComponent(selectedGameObject, components);

        //clean up
        files.clear();
        total = 0;
        helperWindowFunc = nullptr;
    }
}

void GameEditor::showVolumetricLightInfo(Entity *entity)
{
    auto vl = entity->GetComponent<VolumetricLight>();
    auto textures = vl->getMesh()->getTextures();
    for (auto t : *textures)
    {
        ImGui::LabelText("", t->getSourcePath().data());
    }
}

void GameEditor::setHelperWindowFunc(std::function<void()> f)
{
    helperWindowFunc = f;
    ImGui::SetWindowFocus("Helper Window");
}

bool GameEditor::isGameSceneFocused() const
{
    return m_isGameSceneFocused;
}

void GameEditor::createMaterial()
{
    char buf[256];
    ImGui::InputText("Name", buf, 256);
    static std::string selectedShader = "";
    if (ImGui::BeginCombo("Shader", selectedShader.data()))
    {
        for (auto k : Graphics::Shader::collection)
        {
            bool is_selected = (selectedShader == k.first);
            if (ImGui::Selectable(k.first.data(), is_selected))
                selectedShader = k.first;
            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static int total = 0;
    static std::vector<std::string> files;
    static std::vector<int> textureType;
    static std::vector<const char **> selectedTypes;
    static const char *textureTypeSelections[] =
        {
            "Diffuse",
            "Specular",
            "Ambient",
            "Emissive",
            "Height",
            "Normals",
            "Shininess",
            "Opacity",
            "Displacement",
            "Lightmap",
            "Reflection",
            "Unknown",
        };
    for (int i = 0; i < total; i++)
    {
        char file[256];
        std::strcpy(file, files[i].data());
        ImGui::InputText((std::string("Texture") + std::to_string(i)).data(), file, 256);
        makeDropDown(textureTypeSelections, selectedTypes[i], IM_ARRAYSIZE(textureTypeSelections), i);
        textureType[i] = selectedTypes[i] - textureTypeSelections + 1;
        files[i] = file;
    }

    if (ImGui::Button("Add texture"))
    {
        total += 1;
        files.push_back("");
        textureType.push_back(1);
        selectedTypes.push_back(&textureTypeSelections[0]);
    }
    ImGui::SameLine(0, 5);
    if (ImGui::Button("Remove"))
    {
        total -= 1;
        files.pop_back();
        textureType.pop_back();
        selectedTypes.pop_back();
    };

    if (ImGui::Button("Create"))
    {
        std::vector<Graphics::Texture *> textures(total);
        for (int i = 0; i < total; i++)
        {
            textures[i] = Graphics::Texture::add(files[i], files[i], static_cast<Graphics::TextureType>(textureType[i]));
        }
        auto mat = Graphics::Material::add(buf, selectedShader, std::move(textures));

        //clean up
        files.clear();
        textureType.clear();
        selectedTypes.clear();
        total = 0;
        helperWindowFunc = nullptr;
    }
}

void GameEditor::setSelectedGameObject(Entity *newOne)
{
    helperWindowFunc = nullptr;
    selectedGameObject = newOne;
}

} // namespace wlEngine
