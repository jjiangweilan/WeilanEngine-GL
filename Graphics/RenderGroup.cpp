#include "RenderGroup.hpp"
#include "Mesh.hpp"
#include "Material.hpp"

namespace wlEngine
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
    return first->GetRenderingIndex() < second->GetRenderingIndex();
}

} // namespace Graphics

} // namespace wlEngine