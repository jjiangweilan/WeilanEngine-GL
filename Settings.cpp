#include "Settings.hpp"

#include <fstream>
#include <iostream>
#include <sstream>
namespace wlEngine
{
namespace Settings
{
#if SETTINGS_ENGINEMODE
EngineMode engineMode = EngineMode::Editor;
#else
EngineMode engineMode = EngineMode::Gameplay;
#endif
DebugRender debugRender = DebugRender::On;
GameDimension gameDimension = GameDimension::Two;
} // namespace Settings
} // namespace wlEngine
