#include "DebugDraw3D.hpp"
namespace wlEngine
{
namespace Graphics
{
DebugDraw3D::DebugDraw3D()
{
    std::string path = ROOT_DIR + "/Graphics/Material/Shader/DebugDraw/";
    Material::Add("DebugDrawLine", Shader::Add("DebugDrawLine", path + "Debug.vert", "", "", path + "Line.geom", path + "Line.frag"));
    Material::Add("DebugDrawBox", Shader::Add("DebugDrawBox", path + "Debug.vert", "", "", path + "Box.geom", path + "Box.frag"));

    m_mesh.setVertices({
        {glm::vec3(0,0,0), glm::vec2(0.0,0.0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0), glm::vec3(0, 0, 0)},
    }, {
        0
    });
    m_mesh.setMaterial("DebugDrawLine");
}

void DebugDraw3D::drawBox(const glm::vec3& min, const glm::vec3& max, const glm::mat4& model)
{
     
    glBindVertexArray(m_mesh.GetVAO());
	m_mesh.setMaterial("DebugDrawBox");
    m_mesh.GetMaterial()->GetShader()->Use();
    Shader::setUniform(0, model);
    Shader::setUniform(1, min);
    Shader::setUniform(2, max);
    
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
}

void DebugDraw3D::drawLine(const glm::vec3 &from, const glm::vec3 &to, const glm::vec3 &color)
{
    glBindVertexArray(m_mesh.GetVAO());
	m_mesh.setMaterial("DebugDrawLine");
    m_mesh.GetMaterial()->GetShader()->Use();
    Shader::setUniform(0, from);
    Shader::setUniform(1, to);
    Shader::setUniform(2, color);
    glDrawElements(GL_POINTS, 1, GL_UNSIGNED_INT, 0);
}
DebugDraw3D *DebugDraw3D::get()
{
    if (instance)
        return instance;
    instance = new DebugDraw3D();
    return instance;
}
DebugDraw3D *DebugDraw3D::instance = nullptr;
} // namespace Graphics
} // namespace wlEngine