#pragma once
#include <json.hpp>

using Json = nlohmann::json;
namespace KuangyeEngine
{
#define SETTINGS_ENGINEMODE 1     // 1: editor, 0: gameplay
#define SETTINGS_GAME_DIMENSION 0 // 1: 3D, 0: 2D
namespace Settings
{
extern enum class EngineMode : uint8_t {
    Gameplay = 0,
    Editor = 1
} engineMode;

extern enum class DebugRender {
    On,
    Off
} debugRender;

extern enum class GameDimension {
    Two,
    Three
} gameDimension;
} // namespace Settings
} // namespace KuangyeEngine
