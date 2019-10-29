#define GL_GLEXT_PROTOTYPES
#include "PhysicsDebugDraw.hpp"


#include "../EngineManager.hpp"
#include "../System/RenderSystem.hpp"
#include "../Settings.hpp"
#include "../Component/Camera2D.hpp"

#include <SDL.h>
#include <SDL_opengl.h>
#include <vector>
#include <glad/glad.h>
#include <glm/gtc/matrix_transform.hpp>
#include <string>

namespace wlEngine {
    PhysicsDebugDraw::PhysicsDebugDraw() {
		
        windowWidth = RenderSystem::get()->windowWidth;
        windowHeight = RenderSystem::get()->windowHeight;

    }
	void PhysicsDebugDraw::DrawSolidPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

	}
    void PhysicsDebugDraw::DrawPolygon(const b2Vec2* vertices, int32 vertexCount, const b2Color& color) {

    }

    void PhysicsDebugDraw::DrawCircle(const b2Vec2& center, float32 radius, const b2Color& color) {
        B2_NOT_USED(center);
        B2_NOT_USED(radius);
        B2_NOT_USED(color);
    }
    void PhysicsDebugDraw::DrawSolidCircle(const b2Vec2& center, float32 radius, const b2Vec2& axis, const b2Color& color) {
        B2_NOT_USED(center);
        B2_NOT_USED(radius);
        B2_NOT_USED(color);
        B2_NOT_USED(axis);
    }
    void PhysicsDebugDraw::DrawSegment(const b2Vec2& p1, const b2Vec2& p2, const b2Color& color) {
        B2_NOT_USED(p1);
        B2_NOT_USED(p2);
        B2_NOT_USED(color);
    }
    void PhysicsDebugDraw::DrawTransform(const b2Transform& xf) {
        B2_NOT_USED(xf);
    }

    void PhysicsDebugDraw::DrawPoint(const b2Vec2& p, float32 size, const b2Color& color) {
        B2_NOT_USED(p);
        B2_NOT_USED(size);
        B2_NOT_USED(color);
    }
}
