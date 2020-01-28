#include "System/System.hpp"

namespace WeilanEngine
{
std::unordered_set<System *> System::collection = std::unordered_set<System *>();

void System::registerSystem(System *system)
{
	collection.insert(system);
}
} // namespace WeilanEngine
