#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Mesh.hpp"
#include "Shader.hpp"
#include "Material.hpp"
namespace WeilanEngine
{
namespace Graphics
{
class DebugDraw3D
{
public:
    static DebugDraw3D *get();
    void drawLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &color = {0, 1, 0});
    void drawBox(const glm::vec3 &bottomFrontLeft, const glm::vec3 &topBackRight, const glm::mat4 &model);

private:
    Mesh m_mesh;
    DebugDraw3D();
    static DebugDraw3D *instance;
};
} // namespace Graphics
} // namespace WeilanEngine