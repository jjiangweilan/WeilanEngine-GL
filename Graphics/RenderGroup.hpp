#include <set>
#include "Mesh.hpp"

/**
 * @brief RenderGroup is used to store Mesh pointers. The data(Meshe pointers) is sorted in multiset using the Mesh's renderingIndex
 *   
 */

namespace wlEngine
{
namespace Graphics
{

class RenderGroup
{
    struct RenderGroupComp
    {
        bool operator()(Mesh *&first, Mesh *&second)
        {
            return first->GetRenderingIndex() < second->GetRenderingIndex();
        }
    };
    std::multiset<Mesh *, RenderGroupComp> m_data;

public:
    RenderGroup();

    void AddMesh(Mesh *);
    std::multiset<Mesh *, RenderGroupComp>::iterator begin();
    std::multiset<Mesh *, RenderGroupComp>::iterator end();
};
} // namespace Graphics
} // namespace wlEngine