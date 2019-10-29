#pragma once
#ifndef PHYSICS_DEBUG_DRAW_H
#define PHYSICS_DEBUG_DRAW_H

#include <Box2D.h>
#include "../Graphics/Shader.hpp"
namespace wlEngine {
    class PhysicsDebugDraw : public b2Draw {
	public:
		GLuint VAO;
		GLuint VBO;

        PhysicsDebugDraw();
        int windowWidth;
        int windowHeight;
		void DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
		void DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) override;
		void DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) override;
		void DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) override;
		void DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) override;
		void DrawTransform(const b2Transform& xf) override;
		void DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) override;
    };
}
#endif
