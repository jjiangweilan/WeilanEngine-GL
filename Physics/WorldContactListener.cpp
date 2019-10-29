#include "WorldContactListener.hpp"

namespace wlEngine {
    void WorldContactListener::BeginContact(b2Contact* contact) {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        auto rigidBodyA = static_cast<RigidBody*>(fixtureA->GetBody()->GetUserData());
        auto rigidBodyB = static_cast<RigidBody*>(fixtureB->GetBody()->GetUserData());
        if(rigidBodyA->contactBeginCallback)rigidBodyA->contactBeginCallback(fixtureA, fixtureB);
        if(rigidBodyB->contactBeginCallback)rigidBodyB->contactBeginCallback(fixtureB, fixtureA);
    }

    void WorldContactListener::EndContact(b2Contact* contact) {
        auto fixtureA = contact->GetFixtureA();
        auto fixtureB = contact->GetFixtureB();
        auto rigidBodyA = static_cast<RigidBody*>(fixtureA->GetBody()->GetUserData());
        auto rigidBodyB = static_cast<RigidBody*>(fixtureB->GetBody()->GetUserData());
        if(rigidBodyA->contactEndCallback)rigidBodyA->contactEndCallback(fixtureA, fixtureB);
        if(rigidBodyB->contactEndCallback)rigidBodyB->contactEndCallback(fixtureB, fixtureA);
    }
}
