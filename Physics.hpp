#include <vector>
#include "System/PhysicsSystem.hpp"
#include "Component/TRigidbody.hpp"

namespace wlEngine {
    struct Physics {
        static void query(TRigidbody &body);
    };
    } // namespace wlEngine