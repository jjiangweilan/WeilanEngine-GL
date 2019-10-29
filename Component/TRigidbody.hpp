#pragma once
#include "Component.hpp"
#include <glm/glm.hpp>
#include <vector>
#include <functional>
namespace wlEngine {
	#define SpatialHashCellBoxWidth 400;
	#define SpatialHashCellBoxHeight 400;

	enum class ShapeType {
		Circle, Polygon, Line
	};

	struct AABB {
		glm::vec2 max;
		glm::vec2 min;
	};

	struct Cell {
		int row;
		int column;
		bool operator<(const Cell& other) const;
	};

    class Shape {
    public:
        Shape(const glm::vec2& center, const glm::vec2& leftPoint, const glm::vec2& rightPoint);
        virtual ~Shape() = default;
        const glm::vec2 center;
		AABB aabb;
		glm::vec2 leftPoint;
		glm::vec2 rightPoint;
		virtual ShapeType getShapeType() = 0;
    };

    class CircleShape : public Shape{
    public:
        CircleShape(const glm::vec2& center, const float& radius, const glm::vec2& leftPoint = {-1,0}, const glm::vec2& rightPoint = {1, 0});
		virtual ShapeType getShapeType() override;
        float radius;
    };

    class PolygonShape : public Shape {
    public:
        PolygonShape(const float& w, const float& h, const float& angle = 0, const glm::vec2& center = {0,0}, const glm::vec2& leftPoint = {-1,0}, const glm::vec2& rightPoint = {1, 0});
        PolygonShape(const std::vector<glm::vec2>& points, const glm::vec2& center = {0,0}, const glm::vec2& leftPoint = {-1,0}, const glm::vec2& rightPoint = {1, 0});
        std::vector<glm::vec2> points;
		virtual ShapeType getShapeType() override;
        std::vector<glm::vec2> getNormals();
        std::vector<glm::vec2> getPoints();
		glm::vec2 getPolygonCenter();
		void setVertexPos(int& index, glm::vec2&);
		void clearVertex();
    };

    class LineShape : public Shape {
    public:
        LineShape(const std::vector<glm::vec2>& points, const glm::vec2& center = {0,0}, const glm::vec2& leftPoint = {-1,0}, const glm::vec2& rightPoint = {1, 0});
        std::vector<glm::vec2> points;
        virtual ShapeType getShapeType() override;
        std::vector<glm::vec2> getNormals();
        std::vector<glm::vec2> getPoints();
        void setVertexPos(int& index, glm::vec2&);
        void clearVertex();
    };

    enum class BodyType {
        Dynamic, Static
    };

	class TRigidbody;
	using SpatialHash = std::map<Cell, std::vector<TRigidbody*>>;
    using ContactFilter = uint16_t;
    class TRigidbody : public Component {
        COMPONENT_DECLARATION(Component, TRigidbody, 32);
		COMPONENT_EDITABLE_DEC();
    public:
        TRigidbody(Entity *);
        TRigidbody(Entity *, Shape *shape, const BodyType &type);
        TRigidbody(Entity *, void **args);
        ~TRigidbody();

		Shape* shape;
		ContactFilter mask;
	    ContactFilter category;
        bool sensor;
        BodyType type;
        std::vector<TRigidbody *> contactList;
        std::function<void(TRigidbody* body, TRigidbody *other)> contactBegin;
        std::function<void(TRigidbody* body, TRigidbody *other)> contactEnd;
        std::function<bool(TRigidbody *other)> shouldCollide;



        std::vector<Cell> getCells() const;
        void setType(const BodyType &type);
        void setShape(Shape *newShape);
        template <class T> void pushbackVertex(const glm::vec2 &vertex); // expensive
        void clear(); 
        static SpatialHash dynamicSpaticalHash;
        static SpatialHash staticSpaticalHash;
        static void removeFromSpatialHash(TRigidbody *body);
        static void addToSpatialHash(TRigidbody *body);
    };

    template <class T> void TRigidbody::pushbackVertex(const glm::vec2 &vertex) {
        removeFromSpatialHash(this);
        auto pShape = static_cast<T *>(shape);
        pShape->points.emplace_back(vertex.x + pShape->center.x,
                                    vertex.y + pShape->center.y);
        if (vertex.x < pShape->aabb.min.x)
            pShape->aabb.min.x = vertex.x;
        if (vertex.x > pShape->aabb.max.x)
            pShape->aabb.max.x = vertex.x;
        if (vertex.y < pShape->aabb.min.y)
            pShape->aabb.min.y = vertex.y;
        if (vertex.y > pShape->aabb.max.y)
            pShape->aabb.max.y = vertex.y;

        addToSpatialHash(this);
    }
    } // namespace wlEngine
