#include "Utility.hpp"
#include <vector>

#include <wlEngine/wlEngine.hpp>
namespace wlEngine {
namespace Utility {
Json *Utility::findComponentWithName(Json &j, const std::string &name) {
    for (auto &component : j["components"]) {
        if (component["name"] == name) {
            return &component;
        }
    }
    return nullptr;
}

#ifdef _WIN32
std::wstring Utility::convertStringToWstring(const std::string &text) {
    int charNums = MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), NULL, 0);

    Unicode *cVec = new Unicode[charNums];
    MultiByteToWideChar(CP_UTF8, 0, text.c_str(), text.size(), cVec, charNums);
    std::wstring rtn(cVec, cVec + charNums);
    delete[] cVec;
    return rtn;
}
#endif

void Utility::sortEntitiesByZOrder(std::vector<Entity *> &allEntities) { 
	std::sort(allEntities.begin(), allEntities.end(), [](Entity* e1, Entity* e2) {
		return e1->getComponent<Transform>()->position.z < e2->getComponent<Transform>()->position.z;
	}); 
}

std::vector<Entity *>::iterator Utility::findEntity(std::vector<Entity *> &allEntities, Entity *entity) { return std::lower_bound(allEntities.begin(), allEntities.end(), entity); }
} // namespace Utility
} // namespace wlEngine
