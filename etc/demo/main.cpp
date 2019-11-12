#include "Script/ContactFilterDefine.hpp"
#include "Script/Player/PlayerController.hpp"
#include "Script/OpenGLTest.hpp"
#include "System/MonsterSystem.hpp"
#include <wlEngine/wlEngine.hpp>
using namespace wlEngine;

wlEngine::EngineManager *engine = nullptr;

void initGameplaySystem();

class Rotate : public Script
{
    SCRIPT_DECLARATION(Script, Rotate, 2);

public:
    Rotate(Entity *go) : Script(go)
    {
        auto transform = entity->getComponent<Transform>();
        transform->rotate({1, 0, 0}, 25);
    };
    void update() override
    {
        auto transform = entity->getComponent<Transform>();
        transform->rotate({0, 1, 0}, 30 * Time::deltaTime);
        transform->rotateAround({0, 1, 0}, 25 * Time::deltaTime);
		float ratio = (glm::sin(glm::radians(Time::timeAfterGameStart) * 100) + 2) / 2;
        transform->setScale(ratio);
    }
};
SCRIPT_DEFINATION(Script, Rotate, 2);

class Rotate2 : public Script
{
    SCRIPT_DECLARATION(Script, Rotate2, 2);

public:
    Rotate2(Entity *go) : Script(go)
    {
    };
    void update() override
    {
        auto transform = entity->getComponent<Transform>();
        transform->rotate({0, 1, 0}, 30 * Time::deltaTime);
    }
};
SCRIPT_DEFINATION(Script, Rotate2, 2);
int main()
{

    engine = wlEngine::EngineManager::getwlEngine();
    {
        Graphics::Shader::add("tess", "../Shader/Tess.vert", "../Shader/Tess.tesc", "../Shader/Tess.tese", "", "../Shader/Tess.frag");
        Graphics::Shader::add("green", "../Shader/green.vert", "../Shader/green.frag");
        Graphics::Shader::add("sphere", "../Shader/Sphere/Sphere.vert", "../Shader/Sphere/Sphere.tesc", "../Shader/Sphere/Sphere.tese", "", "../Shader/Sphere/Sphere.frag", 4);
        Graphics::Material::add("sphere", "sphere", std::vector<Graphics::Texture *>{Graphics::Texture::add("earth", "../resource/earth-cubemap.png")});
		Graphics::Material::add("basicTree", "green");
        Graphics::Model::add("nanosuit", "../../nano/nanosuit.obj");
        Graphics::Model::add("box", "../../box.fbx");
        Graphics::Model::add("Wood_Steps_Stucco", "../resource/Modular Collection/Modular Village/Wood_Steps_Stucco.obj");
        Graphics::Model::add("Bushes", "../resource/Modular Collection/Modular Terrain Hilly/Prop_Bush_2.obj");

        //add Model manually
        std::vector<GLuint> indices{
            0, 1, 2, 3,
            4, 5, 6, 7,
            8, 9, 10, 11,
            12, 13, 14, 15,
            16, 17, 18, 19,
            20, 21, 22, 23};
        float oneThird = 1 / 3.0;
        std::vector<Graphics::Vertex> vertices{
            {
                {glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, 1.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

                {glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

                {glm::vec3(-0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.0, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

                {glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.25, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

                {glm::vec3(0.5, 0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, -0.5, 0.5), glm::vec3(0, 0, 0), glm::vec2(0.5, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.75, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.75, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},

                {glm::vec3(0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.75, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(0.75, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, -0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(1, oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
                {glm::vec3(-0.5, 0.5, -0.5), glm::vec3(0, 0, 0), glm::vec2(1, 2 * oneThird), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
            }};
        Graphics::Mesh m(std::move(indices), std::move(vertices), Graphics::Material::get("sphere"));
        Graphics::Model::add("Sphere", std::vector<Graphics::Mesh>{m});
    }
    wlEngine::Scene scene;

    engine->setScene(&scene);
    auto camera = scene.createGameObject("Camera", nullptr);
    camera->addComponent<Transform>(0, 0, 0);

    camera->addComponent<Camera3D>();
    scene.setCamera(camera);

    auto test1 = scene.createGameObject("Test", nullptr);
	test1->addComponent<Transform>(0, 0, 0);
    test1->addComponent<Rotate2>();
    auto model = test1->addComponent<Model>("Bushes");
    model->setAllMaterials("basicTree");

    std::vector<Graphics::Texture *> textures = {Graphics::Texture::add("building", "../resource/building.png")};

    auto newMaterial = Graphics::Material::add("building",
                                               "Model",
                                               std::move(textures));

  //  auto box = scene.createGameObject("nanosuit", nullptr);
  //  box->addComponent<Transform>(0, 0, -8);
  //  auto model2 = box->addComponent<Model>("nanosuit");

    auto earth = scene.createGameObject("sphere", nullptr);
    earth->addComponent<Transform>(4.5, 2, 0);
    auto model3 = earth->addComponent<Model>("Sphere");
    earth->addComponent<Rotate>();

    engine->start();
}

void initGameplaySystem()
{
    MonsterSystem::init();
    engine->addGameplaySystem(MonsterSystem::get());
}
