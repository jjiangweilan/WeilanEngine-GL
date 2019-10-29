#include "System.hpp"

namespace  wlEngine {
    std::unordered_set<System*> System::collection = std::unordered_set<System*>();

    void System::registerSystem(System* system) {
        collection.insert(system);
    }
}
