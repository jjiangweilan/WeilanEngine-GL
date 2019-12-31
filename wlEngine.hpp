#ifndef WLENGINE_H
#define WLENGINE_H

#include "EngineManager.hpp"

#include <glm/glm.hpp>
#include <memory>
#include "Component/Component.hpp"
#include "Component/Transform.hpp"
#include "Component/Sprite.hpp"
#include "Component/Script.hpp"
#include "Component/Animation.hpp"
#include "Component/Model.hpp"
#include "Component/Camera2D.hpp"
#include "Component/Camera3D.hpp"
#include "Component/StateMachine.hpp"
#include "Component/Text.hpp"
#include "Component/TRigidbody.hpp"
#include "Component/NpcController.hpp"
#include "Component/LuaScript.hpp"
#include "Component/Audio.hpp"
#include "Component/VolumetricLight.hpp"
#include "Component/RenderScript.hpp"
#include "Component/RenderContext.hpp"

#include "System/System.hpp"
#include "System/RenderSystem.hpp"

#include "GameObject/Scene.hpp"
#include "GameObject/Entity.hpp"

#include "Manager/ResourceManager.hpp"

#include "Graphics/Mesh.hpp"
#include "Graphics/Model.hpp"
#include "Graphics/Material.hpp"
#include "Graphics/Texture.hpp"
#include "Graphics/ShaderParameter.hpp"
#include "Graphics/GlobalShaderParameter.hpp"

#include "Time.hpp"
#include "Input.hpp"
#include "Physics.hpp"
#include "UserInterface.hpp"

#undef main
#endif
