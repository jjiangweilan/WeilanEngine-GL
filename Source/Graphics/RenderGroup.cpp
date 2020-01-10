#include "Graphics/RenderGroup.hpp"
#include "Graphics/Mesh.hpp"
#include "Graphics/Material.hpp"

namespace KuangyeEngine
{
namespace Graphics
{

RenderGroup::RenderGroup() : m_data(){};

void RenderGroup::AddMesh(Mesh *mesh)
{
    m_data.insert(mesh);
}

std::multiset<Mesh *, RenderGroup::RenderGroupComp>::iterator RenderGroup::begin()
{
    return m_data.begin();
}
std::multiset<Mesh *, RenderGroup::RenderGroupComp>::iterator RenderGroup::end()
{
    return m_data.end();
}

bool RenderGroup::RenderGroupComp::operator()(Mesh *first, Mesh *second)
{
    return false;
}

} // namespace Graphics

} // namespace KuangyeEngine