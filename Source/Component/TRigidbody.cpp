#include "Component/TRigidbody.hpp"
#include "Component/Transform.hpp"
#include "GameObject/Entity.hpp"

#include <algorithm>
#include <climits>
namespace WeilanEngine
{
/* Cell **************************/
bool Cell::operator<(const Cell &other) const { return this->row + this->column < other.row + other.column; }

/* Shape ***********************************/
Shape::Shape(const glm::vec2 &center, const glm::vec2 &leftPoint, const glm::vec2 &rightPoint) : center(center), aabb({{-FLT_MAX, -FLT_MAX}, {FLT_MAX, FLT_MAX}}), leftPoint(leftPoint), rightPoint(rightPoint) {}

/* CircleShape *******************************/
CircleShape::CircleShape(const glm::vec2 &center, const float &radius, const glm::vec2 &leftPoint, const glm::vec2 &rightPoint) : Shape(center, leftPoint, rightPoint), radius(radius)
{
    aabb.min = {center.x - radius, center.y - radius};
    aabb.max = {center.x + radius, center.y + radius};
}

ShapeType CircleShape::getShapeType() { return ShapeType::Circle; }

/* PolygonShape *******************************/
PolygonShape::PolygonShape(const float &w, const float &h, const float &angle, const glm::vec2 &center, const glm::vec2 &leftPoint, const glm::vec2 &rightPoint) : Shape(center, leftPoint, rightPoint)
{
    float ww = w / 2;
    float hh = h / 2;
    float a = angle * glm::pi<float>() / 180.0f;
    float s = sin(a);
    float c = cos(a);

    auto rotateFunc = [&](glm::vec2 p) {
        // rotate point
        float xnew = p.x * c - p.y * s;
        float ynew = p.x * s + p.y * c;

        // translate point back:
        p.x = xnew + center.x;
        p.y = ynew + center.y;

        points.push_back(p);
    };
    rotateFunc({ww, hh});
    rotateFunc({ww, -hh});
    rotateFunc({-ww, -hh});
    rotateFunc({-ww, hh});

    aabb.max.x = center.x + ww;
    aabb.max.y = center.y + hh;
    aabb.min.x = center.x - ww;
    aabb.min.y = center.y - hh;
}

PolygonShape::PolygonShape(const std::vector<glm::vec2> &points, const glm::vec2 &center, const glm::vec2 &leftPoint, const glm::vec2 &rightPoint) : Shape(center, leftPoint, rightPoint), points(0)
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;
    for (auto &p : points)
    {
        this->points.emplace_back(p.x + center.x, p.y + center.y);
        if (p.x < minX)
            minX = p.x;
        if (p.x > maxX)
            maxX = p.x;
        if (p.y < minY)
            minY = p.y;
        if (p.y > maxY)
            maxY = p.y;
    }
    aabb.max = {maxX, maxY};
    aabb.min = {minX, minY};
}

ShapeType PolygonShape::getShapeType() { return ShapeType::Polygon; }

std::vector<glm::vec2> PolygonShape::getNormals()
{ // normals won't change! this doesn't need to be dynamically calculated
    if (points.size() < 1)
        return std::vector<glm::vec2>();
    std::vector<glm::vec2> normals(points.size());
    unsigned int i = 0;
    for (; i < points.size() - 1; i++)
    {
        unsigned int next = i + 1;
        normals[i].x = points[i].y - points[next].y;
        normals[i].y = points[next].x - points[i].x;
        normals[i] = glm::normalize(normals[i]);
    }
    normals[i].x = points[i].y - points[0].y;
    normals[i].y = points[0].x - points[i].x;
    normals[i] = glm::normalize(normals[i]);
    return normals;
}

std::vector<glm::vec2> PolygonShape::getPoints()
{
    if (points.size() > 1)
        return points;
    else
        return {{1, 0}, {-1, 0}};
}

void PolygonShape::setVertexPos(int &index, glm::vec2 &newVer)
{
    points[index] = newVer;
    if (newVer.x < aabb.min.x)
        aabb.min.x = newVer.x;
    if (newVer.x > aabb.max.x)
        aabb.max.x = newVer.x;
    if (newVer.y < aabb.min.y)
        aabb.min.y = newVer.y;
    if (newVer.y > aabb.max.y)
        aabb.max.y = newVer.y;
}

void PolygonShape::clearVertex()
{
    points.clear();
    aabb = {{-FLT_MAX, -FLT_MAX}, {FLT_MAX, FLT_MAX}};
}

glm::vec2 PolygonShape::getPolygonCenter()
{
    glm::vec2 pCenter = {0, 0};
    for (auto &p : points)
    {
        pCenter += p;
    }
    pCenter /= points.size();
    return pCenter;
}

/* LineShape ***********************************/
LineShape::LineShape(const std::vector<glm::vec2> &points, const glm::vec2 &center, const glm::vec2 &leftPoint, const glm::vec2 &rightPoint) : Shape(center, leftPoint, rightPoint)
{
    float minX = FLT_MAX;
    float minY = FLT_MAX;
    float maxX = -FLT_MAX;
    float maxY = -FLT_MAX;
    for (auto &p : points)
    {
        this->points.emplace_back(p.x + center.x, p.y + center.y);
        if (p.x < minX)
            minX = p.x;
        if (p.x > maxX)
            maxX = p.x;
        if (p.y < minY)
            minY = p.y;
        if (p.y > maxY)
            maxY = p.y;
    }
    aabb.max = {maxX, maxY};
    aabb.min = {minX, minY};
}
ShapeType LineShape::getShapeType() { return ShapeType::Line; }

std::vector<glm::vec2> LineShape::getNormals()
{
    std::vector<glm::vec2> normals(points.size() - 1);
    for (unsigned int i = 0; i < points.size() - 1; i++)
    {
        unsigned int next = i + 1;
        normals[i].x = points[i].y - points[next].y;
        normals[i].y = points[next].x - points[i].x;
        normals[i] = glm::normalize(normals[i]);
    }
    return normals;
}

std::vector<glm::vec2> LineShape::getPoints()
{
    if (points.size() > 1)
        return points;
    else
        return {{1, 0}, {-1, 0}};
}

void LineShape::setVertexPos(int &index, glm::vec2 &newVer)
{
    points[index] = newVer;
    if (newVer.x < aabb.min.x)
        aabb.min.x = newVer.x;
    if (newVer.x > aabb.max.x)
        aabb.max.x = newVer.x;
    if (newVer.y < aabb.min.y)
        aabb.min.y = newVer.y;
    if (newVer.y > aabb.max.y)
        aabb.max.y = newVer.y;
}

void LineShape::clearVertex()
{
    points.clear();
    aabb = {{-FLT_MAX, -FLT_MAX}, {FLT_MAX, FLT_MAX}};
}

/* TRigidbody ***********************************/
COMPONENT_DEFINATION(Component, TRigidbody, 32);
COMPONENT_EDITABLE_DEF(TRigidbody);

TRigidbody::TRigidbody(Entity *entity, Shape *shape, const BodyType &type) : Component(entity), type(type), mask(0xffff), category(0x1), sensor(false), shape(nullptr)
{
    auto shapeType = shape->getShapeType();
    if (shapeType == ShapeType::Circle)
    {
        this->shape = new CircleShape(*static_cast<CircleShape *>(shape));
    }
    else if (shapeType == ShapeType::Polygon)
    {
        this->shape = new PolygonShape(*static_cast<PolygonShape *>(shape));
    }
    else if (shapeType == ShapeType::Line)
    {
        this->shape = new LineShape(*static_cast<LineShape *>(shape));
    }

    auto cells = getCells();
    if (type == BodyType::Dynamic)
    {
        for (auto &c : cells)
        {
            dynamicSpaticalHash[c].push_back(this);
        }
    }
    else if (type == BodyType::Static)
    {
        for (auto &c : cells)
        {
            staticSpaticalHash[c].push_back(this);
        }
    }
}

TRigidbody::TRigidbody(Entity *entity, void **args) : Component(entity), mask(0xffff), category(0x1), sensor(false), shape(nullptr)
{
    auto &typeStr = *static_cast<std::string *>(args[0]);
    auto &shapeStr = *static_cast<std::string *>(args[1]);
    auto &vertexArr = *static_cast<Json *>(args[2]);
    auto &renderLineArr = *static_cast<Json *>(args[3]);
    auto &categoryAndMask = *static_cast<Json *>(args[4]);
    auto &isSensor = *static_cast<Json *>(args[5]);

    if (typeStr == "static")
        type = BodyType::Static;
    else if (typeStr == "dynamic")
        type = BodyType::Dynamic;

    if (shapeStr == "polygon")
    {
        std::vector<glm::vec2> verexArrTemp;
        for (int i = 0; i < vertexArr.size(); i++)
        {
            verexArrTemp.push_back(glm::vec2(vertexArr[i][0], vertexArr[i][1]));
        }
        PolygonShape *pShape = new PolygonShape(verexArrTemp);
        shape = pShape;
    }
    else if (shapeStr == "circle")
    {
        CircleShape *cShape = new CircleShape({0, 0}, 10);
        shape = cShape;
    }
    else if (shapeStr == "line")
    {
        std::vector<glm::vec2> verexArrTemp;
        for (int i = 0; i < vertexArr.size(); i++)
        {
            verexArrTemp.push_back(glm::vec2(vertexArr[i][0], vertexArr[i][1]));
        }
        LineShape *lShape = new LineShape(verexArrTemp);
        shape = lShape;
    }

    auto cells = getCells();
    if (type == BodyType::Dynamic)
    {
        for (auto &c : cells)
        {
            dynamicSpaticalHash[c].push_back(this);
        }
    }
    else if (type == BodyType::Static)
    {
        for (auto &c : cells)
        {
            staticSpaticalHash[c].push_back(this);
        }
    }

    shape->leftPoint = {renderLineArr[0][0].get<float>(), renderLineArr[0][1].get<float>()};
    shape->rightPoint = {renderLineArr[1][0].get<float>(), renderLineArr[1][1].get<float>()};

    category = categoryAndMask[0].get<int>();
    mask = categoryAndMask[1].get<int>();
    sensor = isSensor.get<int>();
}

TRigidbody::TRigidbody(Entity *entity) : Component(entity)
{
    auto pShape = new PolygonShape({});
    pShape->clearVertex();
    shape = pShape;
    type = BodyType::Static;
    auto cells = getCells();
    for (auto &c : cells)
    {
        staticSpaticalHash[c].push_back(this);
    }
}

void TRigidbody::removeFromSpatialHash(TRigidbody *body)
{
    auto cells = body->getCells();
    if (body->type == BodyType::Static)
    {
        for (auto &c : cells)
        {
            auto &vec = staticSpaticalHash[c];
            for (int i = 0; i < vec.size(); i++)
            {
                if (vec[i] == body)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                }
            }
        }
    }
    else if (body->type == BodyType::Dynamic)
    {
        for (auto &c : cells)
        {
            auto &vec = dynamicSpaticalHash[c];
            for (int i = 0; i < vec.size(); i++)
            {
                if (vec[i] == body)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                }
            }
        }
    }
}

void TRigidbody::addToSpatialHash(TRigidbody *body)
{
    auto cells = body->getCells();
    if (body->type == BodyType::Dynamic)
    {
        for (auto &c : cells)
        {
            dynamicSpaticalHash[c].push_back(body);
        }
    }
    else if (body->type == BodyType::Static)
    {
        for (auto &c : cells)
        {
            staticSpaticalHash[c].push_back(body);
        }
    }
}

void TRigidbody::setType(const BodyType &newType)
{
    removeFromSpatialHash(this);
    type = newType;
    addToSpatialHash(this);
}
void TRigidbody::setShape(Shape *newShape)
{
    delete shape;
    shape = newShape;
}

void TRigidbody::clear()
{
    if (shape->getShapeType() == ShapeType::Polygon)
    {
        static_cast<PolygonShape *>(shape)->clearVertex();
    }
    else if (shape->getShapeType() == ShapeType::Line)
    {
        static_cast<LineShape *>(shape)->clearVertex();
    }
}

TRigidbody::~TRigidbody()
{

    auto cells = getCells();
    if (type == BodyType::Dynamic)
    {
        for (auto &c : cells)
        {
            auto &vec = dynamicSpaticalHash[c];
            for (int i = 0; i < vec.size(); i++)
            {
                if (vec[i] == this)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                }
            }
        }
    }
    else if (type == BodyType::Static)
    {
        for (auto &c : cells)
        {
            auto &vec = staticSpaticalHash[c];
            for (int i = 0; i < vec.size(); i++)
            {
                if (vec[i] == this)
                {
                    vec[i] = vec.back();
                    vec.pop_back();
                }
            }
        }
    }

    delete shape;
}

std::vector<Cell> TRigidbody::getCells() const
{
    std::vector<Cell> cells;
    auto position = entity->GetComponent<Transform>()->position;
    int maxX = position.x + this->shape->aabb.max.x;
    int minX = position.x + this->shape->aabb.min.x;
    int maxY = position.y + this->shape->aabb.max.y;
    int minY = position.y + this->shape->aabb.min.y;

    if (maxX < 0)
        maxX -= 1;
    if (minX < 0)
        minX -= 1;
    if (maxY < 0)
        maxY -= 1;
    if (minY < 0)
        minY -= 1;

    maxX /= SpatialHashCellBoxWidth;
    minX /= SpatialHashCellBoxWidth;
    maxY /= SpatialHashCellBoxHeight;
    minY /= SpatialHashCellBoxHeight;

    for (int r = minY; r <= maxY; r++)
    {
        for (int c = minX; c <= maxX; c++)
        {
            cells.push_back({r, c});
        }
    }
    return cells;
}

SpatialHash TRigidbody::dynamicSpaticalHash = SpatialHash();
SpatialHash TRigidbody::staticSpaticalHash = SpatialHash();
} // namespace WeilanEngine
