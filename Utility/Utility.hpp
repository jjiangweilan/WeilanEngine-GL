#pragma once
#include <json.hpp>
#include <sstream>
#include <string>
#include <vector>
#include <glm/glm.hpp>
#include <random>
#ifdef _WIN32
#include <Windows.h>
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

void getRayFromScreenToWorld(const float& x, const float& y, glm::vec3& origin, glm::vec3& end);

bool TestRayOBBIntersection(
	glm::vec3 ray_origin,        // Ray origin, in world space
	glm::vec3 ray_direction,     // Ray direction (NOT target position!), in world space. Must be normalize()'d.
	glm::vec3 aabb_min,          // Minimum X,Y,Z coords of the mesh when not transformed at all.
	glm::vec3 aabb_max,          // Maximum X,Y,Z coords. Often aabb_min*-1 if your mesh is centered, but it's not always the case.
	glm::mat4 ModelMatrix,       // Transformation applied to the mesh (which will thus be also applied to its bounding box)
	float& intersection_distance // Output : distance between ray_origin and the intersection with the OBB
);
} // namespace Utility
} // namespace wlEngine
