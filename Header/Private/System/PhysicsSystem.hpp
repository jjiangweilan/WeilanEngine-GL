#pragma once
#include "System.hpp"

#include <vector>
#include <map>
#include <glm/glm.hpp>
#include <cfloat>
/* Physics Trigger rules
ps: define A as the main, B is the obejct being collided with A
  1. Collsion will test A.mask & B.catagory or the shouldCollide list, if pass, proceed
  2. if A is dynamic and B is dynmaic.
  The collision will be triggered (A triggers collision)
  The contact function of A will be triggered
  The contact function of B will be triggered only when B wants to contact with A(B.mask & A.catagory != 0)
  3. if A is dynamic and B is static
  The collision will be triggered
  Both contact functions will be triggered

  we have rules 2 and 3 because the system will only actively test dynamic body, static body is passive
*/
namespace WeilanEngine
{
class Entity;
class TRigidbody;
class PhysicsSystem : public System
{
public:
  static PhysicsSystem *get() { return system; }
  static void init();
  void update() override;

private:
  struct CollisionResult
  {
    bool separated = false;
    glm::vec2 penetrationVec = {0, 0};
    CollisionResult(glm::vec2 &penetrationVec, bool separated) : penetrationVec(penetrationVec), separated(separated){};
    CollisionResult(){};
  };
  struct MinMax
  {
    float min = FLT_MAX;
    float max = -FLT_MAX;
  };
  PhysicsSystem();
  bool polygonVsPolygon(TRigidbody *polygon1, TRigidbody *polygon2, glm::vec2 &penetractionVec);
  bool polygonVsCircle(TRigidbody *polygon, TRigidbody *circle, glm::vec2 &penetractionVec);
  bool circleVsCircle(TRigidbody *circle1, TRigidbody *circle2, glm::vec2 &penetractionVec);
  bool lineVsLine(TRigidbody *line1, TRigidbody *line2, glm::vec2 &penetrationVec);
  bool lineVsPolygon(TRigidbody *line, TRigidbody *polygon, glm::vec2 &penetrationVec);
  bool lineVsCircle(TRigidbody *line, TRigidbody *circle, glm::vec2 &penetrationVec);
  void collisionCheck(TRigidbody *body, TRigidbody *other, const bool &otherIsDynamic);
  MinMax getMinMax(const std::vector<glm::vec2> &vecs, const glm::vec2 &normals);

  void contactBeginHelper(TRigidbody *body1, TRigidbody *body2);
  void contactEndHelper(TRigidbody *body1, TRigidbody *body2);
  static PhysicsSystem *system;

  friend struct Physics;
};
} // namespace WeilanEngine
