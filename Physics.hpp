#include "System/PhysicsSystem.hpp"
#include "Component/TRigidbody.hpp"

#include <vector>

namespace wlEngine
{
struct Physics
{
    static void query(TRigidbody &body);
};
} // namespace wlEngine