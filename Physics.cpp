#include "Physics.hpp"

namespace wlEngine
{
void Physics::query(TRigidbody &body)
{
    body.sensor = true;
    assert(body.type == BodyType::Dynamic && "the body type of trigbody needs to be dynamic");
    auto physicsSystem = PhysicsSystem::get();
    for (auto &cell : body.getCells())
    {
        for (auto &other : TRigidbody::dynamicSpaticalHash[cell])
        {
            physicsSystem->collisionCheck(&body, other, true);
        }
        for (auto &other : TRigidbody::staticSpaticalHash[cell])
        {
            physicsSystem->collisionCheck(&body, other, false);
        }
    }

    //this is an overhead
    for (auto &c : body.contactList)
    {

        auto iter = std::find(c->contactList.begin(), c->contactList.end(), &body);
        if (iter != c->contactList.end())
        {
            if (c->contactEnd)
                c->contactEnd(c, &body);

            *iter = c->contactList.back();
            c->contactList.pop_back();
        }
    }
}
} // namespace wlEngine