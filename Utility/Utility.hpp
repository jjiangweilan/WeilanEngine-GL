#pragma once
#include <json.hpp>
#include <sstream>
#include <string>
#include <vector>
#ifdef _WIN32
#include <Windows.h>
#include <random>
#endif
namespace wlEngine {
class Entity;
namespace Utility {
using Unicode = wchar_t;
using Json = nlohmann::json;
template <typename T> std::string toPointerString(T *ptr) {
    std::ostringstream oss;
    oss << ptr;
    return oss.str();
}

Json *findComponentWithName(Json &goJson, const std::string &name);

#ifdef _WIN32
std::wstring convertStringToWstring(const std::string &text);
/* Example */
// auto unicodeVec = Utility::convertStringToUnicodeSet(u8"我");
#endif

void sortEntitiesByZOrder(std::vector<Entity *> &allEntities);
std::vector<Entity *>::iterator findEntity(std::vector<Entity *> &allEntities, Entity *);

template <typename T> T genRandomNumber(const T &min, const T &max) {
    /* Check this https://stackoverflow.com/questions/2704521/generate-random-double-numbers-in-c*/
    std::uniform_real_distribution<float> unif(min, max);
    std::default_random_engine re;
    return unif(re);
}
} // namespace Utility
} // namespace wlEngine
