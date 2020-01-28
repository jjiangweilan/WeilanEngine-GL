#define GL_GLEXT_PROTOTYPES
#include "System/RenderSystem.hpp"

#include "Manager/EngineManager.hpp"

#include "Component/Sprite.hpp"
#include "Component/Transform.hpp"
#include "Component/Animation.hpp"
#include "Component/Model.hpp"
#include "Component/Text.hpp"
#include "Component/TRigidbody.hpp"
#include "Component/VolumetricLight.hpp"
#include "Component/Camera.hpp"
#include "Component/RenderScript.hpp"
#include "Component/CameraController.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Mesh2D.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/DebugDraw3D.hpp"
#include "Graphics/GlobalShaderParameter.hpp"

#include "Global/Input.hpp"

#include "GameEditor/GameEditor.hpp"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_sdl.h>
#include <imgui/imgui_impl_opengl3.h>



namespace WeilanEngine
{
RenderSystem *RenderSystem::renderSystem = nullptr;

RenderSystem::RenderSystem()
{
	// SDL and OpenGL init
	SDLInit();
	ImGuiInit();

	auto shaderPath = ROOT_DIR + "/Extra/Material/Shader/";
	Graphics::Shader::Add("Text", ROOT_DIR + "/Extra/Material/Shader/Text.vert", ROOT_DIR + "/Extra/Material/Shader/Text.frag");
    Graphics::Shader::Add("_Model", ROOT_DIR + "/Extra/Material/Shader/Model.vert", ROOT_DIR + "/Extra/Material/Shader/Model.frag");

	auto globalShaderParam = Graphics::GlobalShaderParameter::Get();
	globalShaderParam->GenBlock("GlobalProjMatrices", 0, 2 * sizeof(glm::mat4), nullptr);
	globalShaderParam->GenBlock("MainCamera", 1, sizeof(glm::vec3), nullptr);
	//uniform buffers
	registerSystem(this);
}

RenderSystem::~RenderSystem()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

RenderSystem *RenderSystem::Get() { return renderSystem; }

void RenderSystem::init() { renderSystem = new RenderSystem(); }

void RenderSystem::render()
{
	auto currentScene = EngineManager::GetWeilanEngine()->GetCurrentScene();
	Camera* mainCamera;
	for (auto &camera : Camera::collection)
	{
		if (!camera->enable || camera->GetEntity()->GetScene() != currentScene) continue;
		if (camera->IsMainCamera())
		{
			mainCamera = camera;
			continue;
		}
		OnRenderScene(camera);
	}
	OnRenderScene(mainCamera);//main camera should be the last camera to render

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT); // clear main frambuffer
	glViewport(0, 0, windowWidth, windowHeight);
	unsigned int id = mainCamera->GetRenderContext()->GetColorAttachment(0)->GetId();
	renderGameEditor(id);

	//End of the frame, swap the window and set back the draw flags in each render node
	SDL_GL_SwapWindow(window);
}

void RenderSystem::Update()
{
	for (auto cameraController : CameraController::collection)
	{
		cameraController->Update();
	}
	render();
}
/*
void RenderSystem::RenderModel(RenderContext *node)
{

}
*/
int RenderSystem::windowResizeCallbackWrapper(void *data, SDL_Event *event)
{
	if (renderSystem)
		return renderSystem->windowResizeCallback(data, event);
	return 0;
}

int RenderSystem::windowResizeCallback(void *data, SDL_Event *event)
{
	if (event->type == SDL_WINDOWEVENT)
	{
		switch (event->window.event)
		{
		case SDL_WINDOWEVENT_RESIZED:
			SDL_GetWindowSize(window, &windowWidth, &windowHeight);
			break;
		}
	}

	return 0;
}

void RenderSystem::setViewPort(int x, int y, int width, int height) { glViewport(x, y, width, height); }

void RenderSystem::SDLInit()
{
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
	SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);
	SDL_SetHint(SDL_HINT_RENDER_DRIVER, "opengl");
	stbi_set_flip_vertically_on_load(true);

	int SDL_Flags = SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE;
#if SETTINGS_ENGINEMODE == 0
	windowWidth = sceneWidth;
	windowHeight = sceneHeight;
#endif
	window = SDL_CreateWindow("Weilan Engine", 150, 50, windowWidth, windowHeight, SDL_Flags);
	glContext = SDL_GL_CreateContext(window);
	SDL_AddEventWatch(windowResizeCallbackWrapper, window);
	gladLoadGLLoader(SDL_GL_GetProcAddress);
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glClearStencil(0);
}

void RenderSystem::ImGuiInit()
{
	ImGui::CreateContext();
	ImGuiIO &io = ImGui::GetIO();

	ImGui::StyleColorsDark();
	ImGui_ImplSDL2_InitForOpenGL(window, &glContext);

	ImGui_ImplOpenGL3_Init("#version 450");
}
#if SETTINGS_ENGINEMODE
void RenderSystem::renderGameEditor(unsigned int &sceneTexId)
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(window);
	ImGui::NewFrame();

	void *data[3];
	data[0] = &sceneTexId;
	data[1] = &sceneWidth;
	data[2] = &sceneHeight;
	GameEditor::get()->render(data);

	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
#endif

/* Render *************/
void RenderSystem::renderGame()
{
	auto currentScene = EngineManager::GetWeilanEngine()->GetCurrentScene();

	for (auto c : Model::collection)
	{
		if (!c->GetEntity()->IsEnable() || c->GetEntity()->GetScene() != currentScene)
			continue;
		render(c);
	}

	for (auto c : Sprite::collection)
	{
		if (!c->GetEntity()->IsEnable() || c->GetEntity()->GetScene() != currentScene)
			continue;
		render(c);
	}

	for (auto text : Text::collection)
	{
		if (!text->GetEntity()->IsEnable())
			continue;
		render(text);
	}
}
void RenderSystem::render(VolumetricLight *vl)
{
	auto vlShader = vl->getShader();
	auto mesh = vl->getMesh();
	auto &vlTextures = *mesh->getTextures();
	vlShader->Use();
	auto transform = vl->GetEntity()->GetComponent<Transform>();
	vlShader->setMat4("model", transform->getModel());
	vlShader->setMat4("view", m_viewMatrix);
	vlShader->setMat4("projection", m_projMatrix);

	int t = vlTextures[0]->GetId();
	glBindVertexArray(mesh->getVAO());
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, t);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
}
void RenderSystem::render(Text *t)
{
	t->getShader()->Use();
	//
	// main texture
	glActiveTexture(GL_TEXTURE0);
	auto model = t->GetEntity()->GetComponent<Transform>()->getModel();
	size_t till = t->renderUntilCharacter();
	for (size_t i = 0; i < till; i++)
	{
		auto &characterETX = t->text[i];
		auto mesh = characterETX.character->getMesh();
		glBindTexture(GL_TEXTURE_2D, mesh->getTextures()->at(0)->GetId());

		t->getShader()->setMat4("model", model * characterETX.getTextTransform());
		t->getShader()->setMat4("view", m_viewMatrix);
		t->getShader()->setMat4("projection", m_projMatrix);
		glBindVertexArray(mesh->getVAO());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	}

	// glBindTexture(GL_TEXTURE_2D, 0);
	// glBindVertexArray(0);
}

void RenderSystem::render(Sprite *t)
{
	if (!t->enable)
		return;

	if (t->draw)
	{
		t->draw(m_viewMatrix, m_projMatrix);
		return;
	}

	t->getShader()->Use();
	if (t->beforeRenderFunc)
		t->beforeRenderFunc();

	auto mesh = t->getMesh();
	auto textures = mesh->getTextures();
	for (int i = 0; i < textures->size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);
		glBindTexture(GL_TEXTURE_2D, textures->at(i)->GetId());
		Graphics::Shader::setUniform(1000 + i, i);
	}

	auto animation = t->GetEntity()->GetComponent<Animation>();
	auto tRigidbody = t->GetEntity()->GetComponent<TRigidbody>();
	auto transform = t->GetEntity()->GetComponent<Transform>();
	if (animation)
		t->getMesh()->clip(animation->getCurrentClip());

	//set the uniform values according to the shader
	Graphics::Shader::setUniform(0, transform->getModel());
	Graphics::Shader::setUniform(1, m_viewMatrix);
	Graphics::Shader::setUniform(2, m_projMatrix);
	Graphics::Shader::setUniform(7, t->transparency);
	if (tRigidbody)
	{
		Graphics::Shader::setUniform(3, 1); // shaTRigidbody;
		glm::vec2 pos = transform->position;
		Graphics::Shader::setUniform(4, tRigidbody->shape->leftPoint + pos);
		Graphics::Shader::setUniform(5, tRigidbody->shape->rightPoint + pos);
	}
	else
	{
		Graphics::Shader::setUniform(3, 0); // shaTRigidbody;
	}
	glBindVertexArray(mesh->getVAO());

	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	if (t->afterRenderFunc)
		t->afterRenderFunc();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindVertexArray(0);
}

void RenderSystem::render(Model *model)
{
	auto gameObject = model->GetEntity();
	auto modelM = model->GetModel();
	if (!modelM)
		return;
	for (auto &mesh : *modelM->GetMeshes())
	{
		auto shader = mesh.GetMaterial()->GetShader();
		shader->Use();
		mesh.GetMaterial()->GetParameters()->Use();
		//
		// draw mesh
		glBindVertexArray(mesh.VAO);
		if (shader->hasTess())
		{
			glPolygonMode(GL_FRONT_AND_BACK, model->GetDrawMode());
			glPatchParameteri(GL_PATCH_VERTICES, shader->getPatches());
			glDrawElements(GL_PATCHES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
		else
		{
			glDrawElements(GL_TRIANGLES, mesh.m_indices.size(), GL_UNSIGNED_INT, 0);
		}
	}
	glBindVertexArray(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
#ifdef DEBUG
	auto aabb = modelM->GetAABB();
	//small offset to prevent collision
	const float offset = 0.001;
	Graphics::DebugDraw3D::get()->drawBox(aabb.min, aabb.max, model->GetEntity()->GetComponent<Transform>()->getModel());
#endif
}

void RenderSystem::PostInit()
{
	m_engine = EngineManager::GetWeilanEngine();
}

void RenderSystem::OnRenderScene(Camera *camera)
{
	// confiture camera relative settings
	auto gsp = Graphics::GlobalShaderParameter::Get();
	auto projIndex = gsp->GetGlobalParameterIndex("GlobalProjMatrices");
	auto cameraIndex = gsp->GetGlobalParameterIndex("MainCamera");
	auto transform = camera->GetEntity()->GetComponent<Transform>();
	auto pos = transform->position;
	auto viewMatrix = camera->GetViewMatrix();
	gsp->SubData(projIndex, 0, sizeof(glm::mat4), &viewMatrix[0][0]);
	gsp->SubData(projIndex, sizeof(glm::mat4), sizeof(glm::mat4), &camera->GetProjMatrix()[0][0]);
	gsp->SubData(cameraIndex, 0, sizeof(glm::vec3), &pos[0]);

	//draw model
	camera->GetRenderContext()->Use();
	auto currentScene = camera->GetEntity()->GetScene();
	for (auto model : WeilanEngine::Model::collection)
	{
		if (!model->GetEntity()->IsEnable() || model->GetEntity()->GetScene() != currentScene)
			continue;

		auto modelM = model->GetModel();
		if (!modelM)
			return;
		auto renderScript = model->GetEntity()->GetComponent<RenderScript>();
		if (renderScript)
			renderScript->Update();

		for (auto &mesh : *modelM->GetMeshes())
		{
			auto material = mesh.GetMaterial();
			auto shader = material->GetShader();
			shader->Use();
			material->GetParameters()->Use();
			//
			// draw mesh
			glBindVertexArray(mesh.GetVAO());
			if (shader->hasTess())
			{
				glPolygonMode(GL_FRONT_AND_BACK, model->GetDrawMode());
				glPatchParameteri(GL_PATCH_VERTICES, shader->getPatches());
				glDrawElements(GL_PATCHES, mesh.GetIndices()->size(), GL_UNSIGNED_INT, 0);
				glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			}
			else
			{
				glDrawElements(GL_TRIANGLES, mesh.GetIndices()->size(), GL_UNSIGNED_INT, 0);
			}
		}
		glBindVertexArray(0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, 0);
		
#ifdef DEBUG
		static bool show = false;
		static bool lastFramePressed = false;
		bool keyPressed = Input::getKeyStatus(SDL_SCANCODE_F11);
		if (keyPressed && !lastFramePressed)
		{
			lastFramePressed = true;
			show = !show;
		}
		if (!keyPressed)
			lastFramePressed = false;
		if (show)
		{
			auto aabb = modelM->GetAABB();
			//small offset to prevent collision
			const float offset = 0.001;
			Graphics::DebugDraw3D::get()->drawBox(aabb.min, aabb.max, model->GetEntity()->GetComponent<Transform>()->getModel());
		}
#endif

	}
}
} // namespace WeilanEngine
