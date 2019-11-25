#pragma once
#include <glm/glm.hpp>
#include <vector>
#include <glad/glad.h>
namespace wlEngine
{
namespace Graphics
{
class Camera
{
    public:
    Camera();
    ~Camera();

    private:
    glm::mat4 projection;
    std::vector<GLuint> framebuffers;
};
} // namespace Graphics
} // namespace wlEngine