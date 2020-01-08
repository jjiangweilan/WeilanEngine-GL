#include "Component/Transform.hpp"
#include "GameObject/Entity.hpp"

namespace wlEngine {
COMPONENT_DEFINATION(Component, Transform, COMPONENT_ALLOCATION_SIZE);
COMPONENT_EDITABLE_DEF(Transform);

Transform::Transform(Entity *gm)
    : Component(gm), position(0.0, 0.0, 0.0), rotation(1.0f), positionMat4(1.0),
      rotateArou(1.0), scaleMat4(1.0), scale(1.0), rotationData{glm::vec3(0.0,0.0,1.0), 0} {}

Transform::Transform(Entity *gm, void **args)
    : Transform(gm) {
    if (args) {
        float x = *static_cast<float *>(args[0]);
        float y = *static_cast<float *>(args[1]);
        float z = *static_cast<float *>(args[2]);
        position = {x, y, z};
        setPosition({x, y, z});
    } else {
        setPosition({0, 0, 0});
    }
}

Transform::Transform(Entity *gm, const float &x, const float &y, const float &z)
    : Transform(gm) {
    position = {x, y, z};
    setLocalPosition({x, y, z});
}
Transform::Transform(Entity *go, const glm::vec3 &coord)
    : Transform(go, coord.x, coord.y, coord.z){};

void Transform::moveBy(const glm::vec3 &vec)
{
    prePosition = position;
    position += vec;

    positionMat4 = glm::translate(glm::mat4(1.0), position);

    for (auto iter = entity->children.begin(); iter != entity->children.end();
         iter++)
    {
        auto t = (*iter)->GetComponent<Transform>();
        t->moveBy(vec);
    }
}
void Transform::moveBy(const float &x, const float &y, const float &z)
{
    prePosition = position;
    position.x += x;
    position.y += y;
    position.z += z;

    positionMat4 = glm::translate(glm::mat4(1.0), position);

    for (auto iter = entity->children.begin(); iter != entity->children.end();
         iter++)
    {
        auto t = (*iter)->GetComponent<Transform>();
        t->moveBy(x, y, z);
    }
}

glm::mat4 Transform::getModel() const
{
    return rotateArou * positionMat4 * rotation * scaleMat4;
}

void Transform::setScale(const float &x, const float &y, const float &z)
{
    scale = {x, y, z};
    scaleMat4 = glm::scale(glm::mat4(1.0), {x, y, z});
}
void Transform::SetScale(const float &ratio)
{
    scale = {ratio, ratio, ratio};
    scaleMat4 = glm::scale(glm::mat4(1.0), scale);
}
void Transform::Rotate(const glm::vec3 &axis, const float &degree)
{
    rotationData.axis = axis;
    rotationData.degree = degree;
    rotation = glm::rotate(glm::mat4(1.0), glm::radians(degree), axis);
}

void Transform::setRotation(const glm::mat4 &ro) { rotation = ro; }

void Transform::rotateAround(const glm::vec3 &axis, const float &degree)
{
    rotateArou = glm::rotate(rotateArou, glm::radians(degree), axis);
}

void Transform::setPosition(const glm::vec3 &pos)
{
    prePosition = position;
    glm::vec3 moveVector = pos - position;
    position = pos;

    positionMat4 = glm::translate(glm::mat4(1.0), position);

    for (auto iter = entity->children.begin(); iter != entity->children.end();
         iter++)
    {
        auto t = (*iter)->GetComponent<Transform>();
        if (t)
            t->moveBy(moveVector.x, moveVector.y, moveVector.z);
    }
}

void Transform::setToPreviousPosition()
{
    glm::vec3 moveVector = prePosition - position;
    position = prePosition;

    positionMat4 = glm::translate(glm::mat4(1.0), position);

    for (auto iter = entity->children.begin(); iter != entity->children.end();
         iter++)
    {
        auto t = (*iter)->GetComponent<Transform>();
        if (t)
            t->moveBy(moveVector.x, moveVector.y, moveVector.z);
    }
}

void Transform::setLocalPosition(const glm::vec3 &pos)
{
    prePosition = position;
    glm::vec3 newPosition = (entity->parent ? entity->parent->GetComponent<Transform>()->position : glm::vec3(0.0)) + pos;
    glm::vec3 moveVector = newPosition - position;

    position = newPosition;

    positionMat4 = glm::translate(glm::mat4(1.0), position);

    for (auto iter = entity->children.begin(); iter != entity->children.end(); iter++)
    {
        auto t = (*iter)->GetComponent<Transform>();
        if (t)
            t->moveBy(moveVector.x, moveVector.y, moveVector.z);
    }
}

glm::vec3 Transform::getLocalPosition()
{
    if (entity->parent)
        return position - entity->parent->GetComponent<Transform>()->position;
    else
        return position;
}

} // namespace wlEngine
