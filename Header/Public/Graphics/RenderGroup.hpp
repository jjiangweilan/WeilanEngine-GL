#pragma once
#include <set>

/**
 * @brief RenderGroup is used to store Mesh pointers. The data(Meshe pointers) is sorted in multiset using the Mesh's renderingIndex
 *   
 */

namespace KuangyeEngine
{
namespace Graphics
{
class Mesh;
class RenderGroup
{
    struct RenderGroupComp
    {
        bool operator()(Mesh *first, Mesh *second);
    };
    std::multiset<Mesh *, RenderGroupComp> m_data;

public:
    RenderGroup();

    void AddMesh(Mesh *);
    std::multiset<Mesh *, RenderGroupComp>::iterator begin();
    std::multiset<Mesh *, RenderGroupComp>::iterator end();
};
} // namespace Graphics
} // namespace KuangyeEngine