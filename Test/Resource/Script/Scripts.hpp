#pragma once
#include "Global/KuangyeEngine.hpp"

using namespace KuangyeEngine;
#define UNIFORM_BLOCK_INDEX_PROJECTION_MATRICS 0
#define UNIFORM_BLOCK_INDEX_MAIN_CAMERA 1
class Rotate : public Script
{
    SCRIPT_DECLARATION(Script, Rotate, 2);

public:
    Rotate(Entity *go) : Script(go)
    {
        auto transform = entity->GetComponent<Transform>();
        transform->Rotate({1, 0, 0}, 25);
    };
    void update() override
    {
        auto transform = entity->GetComponent<Transform>();
        transform->Rotate({0, 1, 0}, 30 * Time::deltaTime);
        transform->rotateAround({0, 1, 0}, 25 * Time::deltaTime);
        float ratio = (glm::sin(glm::radians(Time::timeAfterGameStart) * 100) + 2) / 2;
        transform->SetScale(ratio);
    }
};
SCRIPT_DEFINATION(Script, Rotate, 2);

class Rotate2 : public Script
{
    SCRIPT_DECLARATION(Script, Rotate2, 2);

public:
    Rotate2(Entity *go) : Script(go){};
    void update() override
    {
        auto transform = entity->GetComponent<Transform>();
        transform->Rotate({0, 1, 0}, 30 * Time::deltaTime);
    }
};
SCRIPT_DEFINATION(Script, Rotate2, 2);

class ModelUniformUpdate : public RenderScript
{
    RENDERSCRIPT_DECLARATION(RenderScript, ModelUniformUpdate, 16);

public:
    std::vector<glm::mat4 *> models;
    std::vector<glm::vec3 *> lightPos;
    ModelUniformUpdate(Entity *entity) : RenderScript(entity), models(){};

    Entity* light;
    void Update() override
    {
        auto model = entity->GetComponent<Transform>()->getModel();
        for (auto& m : models)
            *m = model;

        for (auto& lightVal : lightPos)
            *lightVal = light->GetComponent<Transform>()->position;
    };
    void PostInit() override
    {
        for (auto& mesh : *entity->GetComponent<Model>()->GetModel()->GetMeshes())
        {
            models.push_back(mesh.GetMaterial()->GetParameters()->GetParameter<glm::mat4>("model"));
            *models.back() = entity->GetComponent<Transform>()->getModel();

            if (light)
            {
                auto lightPosVal = mesh.GetMaterial()->GetParameters()->GetParameter<glm::vec3>("light1.position");
                if(lightPosVal)
                {
                    lightPos.push_back(lightPosVal);
                    *lightPosVal = light->GetComponent<Transform>()->position;
                }
            }
        }
    }
};
RENDERSCRIPT_DEFINATION(RenderScript, ModelUniformUpdate, 16);

class WaveRenderUpdate : public RenderScript
{
    RENDERSCRIPT_DECLARATION(RenderScript, WaveRenderUpdate, 1);

public:
    std::vector<float *> times;
    std::vector<glm::mat4 *> models;
    std::vector<glm::vec3*> lightPos;
    WaveRenderUpdate(Entity *entity) : RenderScript(entity), times(){};


    Entity* light;

    void Update() override
    {
		for (auto time : times)
            *time = Time::timeAfterGameStart;

        auto model = entity->GetComponent<Transform>()->getModel();
        for (auto m : models)
            *m = model;
        
        for (auto lightVal : lightPos)
        {
            *lightVal = light->GetComponent<Transform>()->position;
        }
        
    };
    void PostInit() override
    {
		times.clear();
		models.clear();
        lightPos.clear();
        for (auto& mesh : *entity->GetComponent<Model>()->GetModel()->GetMeshes())
        {
			auto p = mesh.GetMaterial()->GetParameters()->GetParameter<float>("t");
			if (p)
			{
				times.push_back(p);
				*p = Time::timeAfterGameStart;
			}

			auto model = mesh.GetMaterial()->GetParameters()->GetParameter<glm::mat4>("model");
			if (model)
			{
				models.push_back(model);
				*model = entity->GetComponent<Transform>()->getModel();
			}

            auto lightVal = mesh.GetMaterial()->GetParameters()->GetParameter<glm::vec3>("light1.position");
            if (lightVal)
            {
                lightPos.push_back(lightVal);
                *lightVal = light->GetComponent<Transform>()->position;
            }
        }
    }
};
RENDERSCRIPT_DEFINATION(RenderScript, WaveRenderUpdate, 1);

class CameraRenderUpdate : public RenderScript
{
    RENDERSCRIPT_DECLARATION(RenderScript, CameraRenderUpdate, 2)
public:
    CameraRenderUpdate(Entity *entity) : RenderScript(entity) {
        
    }
    void Update() override
    {
        static const auto gsp = Graphics::GlobalShaderParameter::Get();
        static const auto projIndex = gsp->GetGlobalParameterIndex("GlobalProjMatrices");
        static const auto cameraIndex = gsp->GetGlobalParameterIndex("MainCamera");
        auto transform = entity->GetComponent<Transform>();
        auto camera3d = entity->GetComponent<Camera3D>();
		camera3d->update();
        auto pos = transform->position;
        auto viewMatrix = camera3d->GetViewMatrix();
        auto projMatrix = camera3d->GetProjMatrix();
        gsp->SubData(projIndex, 0, sizeof(glm::mat4), &viewMatrix[0][0]);
        gsp->SubData(projIndex, sizeof(glm::mat4), sizeof(glm::mat4), &projMatrix[0][0]);
        gsp->SubData(cameraIndex, 0, sizeof(glm::vec3), &pos[0]);
    }
};
RENDERSCRIPT_DEFINATION(RenderScript, CameraRenderUpdate, 2)
