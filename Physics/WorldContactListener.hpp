#ifndef WORLD_CONTACT_LISTENER_H
#define WORLD_CONTACT_LISTENER_H

#include <Box2D.h>

#include "../Component/RigidBody.hpp"

namespace wlEngine {
    class WorldContactListener : public b2ContactListener {
        void BeginContact(b2Contact* contact);
        void EndContact(b2Contact* contact);
        void PresolveContact(b2Contact* contact);
        
    };
}

#endif


