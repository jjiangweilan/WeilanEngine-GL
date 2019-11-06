#include "OnScreenText.hpp"
#include "../EngineManager.hpp"
namespace wlEngine {
    void OnScreenText::init() {
        m_entity = EngineManager::getwlEngine()->getCurrentScene()->createGameObject("_OnScreenText", nullptr);
    }
}