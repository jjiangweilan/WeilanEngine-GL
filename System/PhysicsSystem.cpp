#include "PhysicsSystem.hpp"
#include "../EngineManager.hpp"
#include "../Component/TRigidbody.hpp"
#include "../Component/Transform.hpp"
#include "../GameObject/Entity.hpp"
#include "../Utility/Utility.hpp"
extern wlEngine::Entity *player;
namespace wlEngine {
PhysicsSystem *PhysicsSystem::system = nullptr;

PhysicsSystem::PhysicsSystem() : System() {
    registerSystem(this);
}

void PhysicsSystem::init() {
    if (!system) {
        system = new PhysicsSystem();
    }
}

void PhysicsSystem::update() {
    /*for (auto& r : RigidBody::collection) {
        auto transform = r->entity->getComponent<Transform>();
        transform->setPosition(r->getPosition());
    }*/

    // update cells
    auto scene = EngineManager::getwlEngine()->getCurrentScene();
    TRigidbody::dynamicSpaticalHash.clear();
    for (auto &body : TRigidbody::collection) {
        if (!body->entity->isEnable() || body->entity->getScene() != scene)
            continue;
        if (body->type == BodyType::Dynamic) {
            for (auto &cell : body->getCells()) {
                TRigidbody::dynamicSpaticalHash[cell].push_back(body);
            }
        }
    }

    // collision detection
    for (auto &body : TRigidbody::collection) {
        if (!body->entity->isEnable() || body->entity->getScene() != scene)
            continue;
        if (body->type == BodyType::Dynamic) {
            for (auto &cell : body->getCells()) {
                for (auto &other : TRigidbody::dynamicSpaticalHash[cell]) {
                    collisionCheck(body, other, true);
                }
                for (auto &other : TRigidbody::staticSpaticalHash[cell]) {
                    collisionCheck(body, other, false);
                }
            }
        }
    }
}
PhysicsSystem::MinMax
PhysicsSystem::getMinMax(const std::vector<glm::vec2> &vecs,
                         const glm::vec2 &normal) {
    MinMax mm;
    for (auto &v : vecs) {
        float mag = glm::dot(v, normal);
        if (mag < mm.min) {
            mm.min = mag;
        }
        if (mag > mm.max) {
            mm.max = mag;
        }
    }
    return mm;
}

bool PhysicsSystem::polygonVsPolygon(TRigidbody *polygon1, TRigidbody *polygon2,
                                     glm::vec2 &penetrationVec) {
    PolygonShape *shape1 = static_cast<PolygonShape *>(polygon1->shape);
    PolygonShape *shape2 = static_cast<PolygonShape *>(polygon2->shape);

    auto points1 = shape1->getPoints();
    auto points2 = shape2->getPoints();
    Transform *t1 = polygon1->entity->getComponent<Transform>();
    Transform *t2 = polygon2->entity->getComponent<Transform>();
    glm::vec2 pos1 = t1->position;
    glm::vec2 pos2 = t2->position;

    for (auto &p : points1)
        p += pos1;
    for (auto &p : points2)
        p += pos2;

    float overlap = FLT_MAX;

    auto checkFunc = [&](std::vector<glm::vec2> &normals, bool fromMain) {
        for (auto &n : normals) {
            MinMax mm1 = getMinMax(points1, n);
            MinMax mm2 = getMinMax(points2, n);

            float test1 = mm1.min - mm2.max;
            float test2 = mm2.min - mm1.max;

            if (test1 > 0 || test2 > 0) {
                contactEndHelper(polygon1, polygon2);
                return true;
            }

            // calculate penetration
            float overlapNew = test1 > test2 ? -test1 : -test2;
            if (overlapNew < overlap) {
                overlap = overlapNew;
                penetrationVec = fromMain && overlap == -test2 ? -n : n;
            }
        }

        return false;
    };

    auto normals1 = shape1->getNormals();
    if (checkFunc(normals1, true))
        return true;
    auto normals2 = shape2->getNormals();
    if (checkFunc(normals2, false))
        return true;

    contactBeginHelper(polygon1, polygon2);

    penetrationVec *= overlap;
    return false;
}
bool PhysicsSystem::polygonVsCircle(TRigidbody *polygon, TRigidbody *circle,
                                    glm::vec2 &penetrationVec) {
    PolygonShape *polygonShape = static_cast<PolygonShape *>(polygon->shape);
    CircleShape *circleShape = static_cast<CircleShape *>(circle->shape);
    Transform *t1 = polygon->entity->getComponent<Transform>();
    Transform *t2 = circle->entity->getComponent<Transform>();
    auto circlePos = circleShape->center + glm::vec2(t2->position);
    float radius = circleShape->radius;
    auto points = polygonShape->points;
    auto polyPos = polygonShape->center + glm::vec2(t1->position);
    // add the offset and find closest point
    float distance2 = FLT_MAX;
    glm::vec2 closestAxis;
    for (auto &p : points) {
        p += polyPos;
        auto distanceVecNew = circlePos - p;
        float distance2New = distanceVecNew.x * distanceVecNew.x +
                             distanceVecNew.y * distanceVecNew.y;
        if (distance2New < distance2) {
            distance2 = distance2New;
            closestAxis = distanceVecNew;
        }
    }

    auto axies = polygonShape->getNormals();
    // circle to the closest point
    closestAxis = glm::normalize(closestAxis);
    axies.push_back(closestAxis);

    float overlap = FLT_MAX;

    for (auto &n : axies) {
        auto polyMM = getMinMax(points, n);
        auto circleProject = glm::dot(circlePos, n);
        float circleProjMin = circleProject - radius;
        float circleProjMax = circleProject + radius;

        float test1 = circleProjMin - polyMM.max;
        float test2 = polyMM.min - circleProjMax;
        if (test1 > 0 || test2 > 0) {
            contactEndHelper(polygon, circle);
            return true;
        }

        // calculate penetration
        float overlapNew = test1 > test2 ? -test1 : -test2;
        if (overlapNew < overlap) {
            overlap = overlapNew;
            penetrationVec = n;
        }
    }

    contactBeginHelper(polygon, circle);
    penetrationVec *= overlap;
    return false;
}

bool PhysicsSystem::circleVsCircle(TRigidbody *circle1, TRigidbody *circle2,
                                   glm::vec2 &penetrationVec) {
    auto t1 = circle1->entity->getComponent<Transform>();
    auto t2 = circle2->entity->getComponent<Transform>();
    auto circleShape1 = static_cast<CircleShape *>(circle1->shape);
    auto circleShape2 = static_cast<CircleShape *>(circle2->shape);
    float radius1 = circleShape1->radius;
    float radius2 = circleShape2->radius;
    auto distanceVec = glm::vec2(t1->position) + circleShape1->center -
                       (glm::vec2(t2->position) + circleShape2->center);
    float dis = glm::length(distanceVec);
    if (dis > radius1 + radius2) {
        return true;
    }

    float overlap = radius1 + radius2 - dis;
    penetrationVec = distanceVec / dis * overlap;
    return false;
}

bool PhysicsSystem::lineVsLine(TRigidbody *line1, TRigidbody *line2,
                               glm::vec2 &penetrationVec) {
    LineShape *shape1 = static_cast<LineShape *>(line1->shape);
    LineShape *shape2 = static_cast<LineShape *>(line2->shape);

    auto points1 = shape1->getPoints();
    auto points2 = shape2->getPoints();
    Transform *t1 = line1->entity->getComponent<Transform>();
    Transform *t2 = line2->entity->getComponent<Transform>();
    glm::vec2 pos1 = t1->position;
    glm::vec2 pos2 = t2->position;

    for (auto &p : points1)
        p += pos1;
    for (auto &p : points2)
        p += pos2;

    float overlap = FLT_MAX;

    bool separated = true;
    for (unsigned int i = 0; i < points1.size() - 1; i++) {
        unsigned int next = i + 1;
        glm::vec2 normal;
        normal.x = points1[i].y - points1[next].y;
        normal.y = points1[next].x - points1[i].x;
        normal = glm::normalize(normal);
        auto inverseNormal = glm::normalize(points1[next] - points1[i]);

        float proj1 = glm::dot(points1[i], normal);
        float parallelMax = glm::dot(points1[next], inverseNormal);
        float parallelMin = glm::dot(points1[i], inverseNormal);

        MinMax mm2 = getMinMax(points2, normal);
        MinMax parallelmm = getMinMax(points2, inverseNormal);
        float test1 = mm2.max - proj1;
        float test2 = proj1 - mm2.min;
        bool test3 = parallelMax < parallelmm.min;
        bool test4 = parallelMin > parallelmm.max;

        if (test1 > 0 && test2 > 0 && !test3 && !test4) {
            contactEndHelper(line1, line2);
            // calculate penetration
            float overlapNew = test1 < test2 ? test1 : test2;
            if (overlapNew < overlap) {
                overlap = overlapNew;
                penetrationVec = test1 < test2 ? -normal : normal;
            }
            separated = false;
            break;
        }
    }

    if (!separated) {
        contactBeginHelper(line1, line2);
        penetrationVec *= overlap;
    }

    return separated;
}

bool PhysicsSystem::lineVsPolygon(TRigidbody *line, TRigidbody *polygon,
                                  glm::vec2 &penetrationVec) {
    LineShape *shape1 = static_cast<LineShape *>(line->shape);
    PolygonShape *shape2 = static_cast<PolygonShape *>(polygon->shape);

    auto points1 = shape1->getPoints();
    auto points2 = shape2->getPoints();
    Transform *t1 = line->entity->getComponent<Transform>();
    Transform *t2 = polygon->entity->getComponent<Transform>();
    glm::vec2 pos1 = t1->position;
    glm::vec2 pos2 = t2->position;

    for (auto &p : points1)
        p += pos1;
    for (auto &p : points2)
        p += pos2;

    auto normals = shape2->getNormals();

    bool separated = true;
    glm::vec2 penetrationLineVec = {0, 0};
    glm::vec2 penetrationPolygonVec = {0, 0};
    static std::vector<glm::vec2> lineTemp(2); // no multi thread
    float lastLineOverlap = -FLT_MAX;
    for (unsigned int i = 0; i < points1.size() - 1; i++) {
        unsigned int next = i + 1;
        glm::vec2 normal;
        normal.x = points1[i].y - points1[next].y;
        normal.y = points1[next].x - points1[i].x;
        normal = glm::normalize(normal);

        float proj1 = glm::dot(points1[i], normal);

        MinMax mm2 = getMinMax(points2, normal);
        float test1 = mm2.max - proj1;
        float test2 = proj1 - mm2.min;

        glm::vec2 vertexPenetrationVec = {0, 0};
        lineTemp[0] = points1[i];
        lineTemp[1] = points1[next];
        if (test1 > 0 && test2 > 0) {
            // more test
            float overlap = FLT_MAX;
            int i = 0;
            bool isSeparated = false;
            for (; i < normals.size(); i++) {
                auto lineMM = getMinMax(lineTemp, normals[i]);
                auto polygonMM = getMinMax(points2, normals[i]);

                float test5 = lineMM.min - polygonMM.max;
                float test6 = polygonMM.min - lineMM.max;

                if (test5 > 0 ||
                    test6 > 0) { // separated, this line is separated from the
                                 // polygon, go to next line
                    isSeparated = true;
                    break;
                }

                // need a penetration vec to compare with the one below
                float overlapNew = test5 > test6 ? -test5 : -test6;
                if (overlapNew < overlap) {
                    if (overlapNew == 0)
                        overlapNew = 0.1;
                    vertexPenetrationVec =
                        test5 < test6 ? normals[i] : -normals[i];
                    overlap = overlapNew;
                }
            }
            if (!isSeparated) {
                if (overlap > lastLineOverlap) {
                    lastLineOverlap = overlap;
                    penetrationPolygonVec = (vertexPenetrationVec * overlap);
                }
                // calculate penetration
                float overlap2 = test1 < test2 ? test1 : test2;
                glm::vec2 penetrationVecTemp = {0, 0};
                penetrationVecTemp = test1 < test2 ? -normal : normal;
                penetrationVecTemp *= overlap2;

                penetrationLineVec += penetrationVecTemp;
                separated = false;
            }
        }
    }
    if (separated) {
        contactEndHelper(line, polygon);
        return true;
    }

    contactBeginHelper(line, polygon);
    if (penetrationLineVec.x * penetrationLineVec.x +
            penetrationLineVec.y * penetrationLineVec.y <
        penetrationPolygonVec.x * penetrationPolygonVec.x +
            penetrationPolygonVec.y * penetrationPolygonVec.y)
        penetrationVec = penetrationLineVec;
    else {
        penetrationVec = penetrationPolygonVec;
    }

    return false;
}

bool PhysicsSystem::lineVsCircle(TRigidbody *line, TRigidbody *circle,
                                 glm::vec2 &penetrationVec) {
    LineShape *shape1 = static_cast<LineShape *>(line->shape);
    CircleShape *shape2 = static_cast<CircleShape *>(circle->shape);

    auto points1 = shape1->getPoints();
    Transform *t1 = line->entity->getComponent<Transform>();
    Transform *t2 = circle->entity->getComponent<Transform>();
    glm::vec2 pos1 = t1->position;
    glm::vec2 pos2 = t2->position;

    for (auto &p : points1)
        p += pos1;

    float overlap = FLT_MAX;

    // add the offset and find closest point
    float distance2 = FLT_MAX;
    float radius2 = shape2->radius * shape2->radius;
    glm::vec2 closestAxis;
    for (auto &p : points1) {
        auto distanceVecNew = pos2 - p;
        float distance2New = distanceVecNew.x * distanceVecNew.x +
                             distanceVecNew.y * distanceVecNew.y;
        if (distance2New < distance2) {
            distance2 = distance2New;
            closestAxis = distanceVecNew;
        }

        if (distance2 < radius2) {
            contactBeginHelper(line, circle);
            penetrationVec = glm::normalize(closestAxis) * glm::sqrt(distance2);
            return false;
        }
    }

    bool separated = true;
    for (unsigned int i = 0; i < points1.size() - 1; i++) {
        unsigned int next = i + 1;
        glm::vec2 normal;
        normal.x = points1[i].y - points1[next].y;
        normal.y = points1[next].x - points1[i].x;
        normal = glm::normalize(normal);
        auto inverseNormal = glm::normalize(points1[next] - points1[i]);

        float proj1 = glm::dot(points1[i], normal);
        float parallelMax = glm::dot(points1[next], inverseNormal);
        float parallelMin = glm::dot(points1[i], inverseNormal);
        float circleDot = glm::dot(pos2, normal);
        float circleParallelDot = glm::dot(pos2, inverseNormal);
        MinMax mm2;
        mm2.max = circleDot + shape2->radius;
        mm2.min = circleDot - shape2->radius;

        float test1 = mm2.max - proj1;
        float test2 = proj1 - mm2.min;
        bool test3 = parallelMax < circleParallelDot - shape2->radius;
        bool test4 = parallelMin > circleParallelDot + shape2->radius;

        if (test1 > 0 && test2 > 0 && !test3 && !test4) {
            contactEndHelper(line, circle);
            // calculate penetration
            float overlapNew = test1 < test2 ? test1 : test2;
            if (overlapNew < overlap) {
                overlap = overlapNew;
                penetrationVec = test1 < test2 ? -normal : normal;
            }
            separated = false;
            break;
        }
    }

    if (!separated) {
        contactBeginHelper(line, circle);
        penetrationVec *= overlap;
    }

    return separated;
}

void PhysicsSystem::collisionCheck(TRigidbody *body, TRigidbody *other,
                                   const bool &otherIsDynamic) {
    if (body == other || (body->mask & other->category) == 0)//((body->category & other->mask) == 0 ))//|
        return;
    if (body->shouldCollide && !body->shouldCollide(other)) return;
    ShapeType thisShape = body->shape->getShapeType();
    ShapeType otherShape = other->shape->getShapeType();
    CollisionResult cr;
    if (thisShape == ShapeType::Polygon) {
        switch (otherShape) {
        case ShapeType::Polygon:
            cr.separated = polygonVsPolygon(body, other, cr.penetrationVec);
            break;
        case ShapeType::Circle:
            cr.separated = polygonVsCircle(body, other, cr.penetrationVec);
            break;
        case ShapeType::Line:
            cr.separated = lineVsPolygon(other, body, cr.penetrationVec);
            break;
        }
    } else if (thisShape == ShapeType::Circle) {
        switch (otherShape) {
        case ShapeType::Polygon:
            cr.separated = polygonVsCircle(other, body, cr.penetrationVec);
            break;
        case ShapeType::Circle:
            cr.separated = circleVsCircle(body, other, cr.penetrationVec);
            break;
        case ShapeType::Line:
            cr.separated = lineVsCircle(other, body, cr.penetrationVec);
            break;
        }
    } else if (thisShape == ShapeType::Line) {
        switch (otherShape) {
        case ShapeType::Polygon:
            cr.separated = lineVsPolygon(body, other, cr.penetrationVec);
            break;
        case ShapeType::Circle:
            cr.separated = lineVsCircle(body, other, cr.penetrationVec);
            break;
        case ShapeType::Line:
            cr.separated = lineVsLine(body, other, cr.penetrationVec);
            break;
        }
    }

    if (cr.separated || body->sensor || other->sensor || !body->entity)
        return;
    if (otherIsDynamic) {
        cr.penetrationVec /= 2;
        body->entity->getComponent<Transform>()->moveBy(cr.penetrationVec.x,
                                                        cr.penetrationVec.y, 0);
        other->entity->getComponent<Transform>()->moveBy(
            -cr.penetrationVec.x, -cr.penetrationVec.y, 0);
    } else
        body->entity->getComponent<Transform>()->moveBy(cr.penetrationVec.x,
                                                        cr.penetrationVec.y, 0);
}

void PhysicsSystem::contactEndHelper(TRigidbody *body1, TRigidbody *body2) {
    //find body2 in body1's contact list
    auto p2 =
        std::find(body1->contactList.begin(), body1->contactList.end(), body2);

    //if body2 is not in body1's contact list
    if (p2 != body1->contactList.end()) {
        *p2 = body1->contactList.back();
        body1->contactList.pop_back();

        if (body1->contactEnd)
            body1->contactEnd(body1, body2);

        if (body2->type == BodyType::Static || body2->mask & body1->category) {
            auto p1 = std::find(body2->contactList.begin(), body2->contactList.end(), body1);
            *p1 = body2->contactList.back();
            body2->contactList.pop_back();
            if (body2->contactEnd)
                body2->contactEnd(body2, body1);
        }
    }
}
void PhysicsSystem::contactBeginHelper(TRigidbody *body1, TRigidbody *body2) {
    // find body2 from body1's contact list
    auto p2 = std::find(body1->contactList.begin(), body1->contactList.end(), body2);

    // if body2 is not in body1's contact list
    if (p2 == body1->contactList.end()) {
        body1->contactList.push_back(body2);
        if (body1->contactBegin)
            body1->contactBegin(body1, body2);

        if (body2->type == BodyType::Static || body2->mask & body1->category) {
            body2->contactList.push_back(body1);
            if (body2->contactBegin)
                body2->contactBegin(body2, body1);
        }
    }
}

} // namespace wlEngine
