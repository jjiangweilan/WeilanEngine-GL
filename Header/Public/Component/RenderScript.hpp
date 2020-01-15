#include "Component.hpp"
/**
 * @brief Render Script will be called before a drawable component drew to screen,
 * or before a RenderContext's draw call is called
 * Render Script acts like script but it's one per entity. It's used in RenderSystem 
 * Use Render Script to update unique parameter to material. Or you should clone the material
 * 
 */
namespace WeilanEngine
{
struct RenderScript : public Component
{
    static const std::size_t componentId;
    virtual size_t getId() override;
    virtual bool isType(const std::size_t &typeId) const override;
    static std::set<RenderScript *> collection;
    RenderScript(Entity *entity) : Component(entity){};
    virtual void Update(){};
    virtual void PostInit(){};
};

#define RENDERSCRIPT_DECLARATION(P, T, N)                                       \
public:                                                                         \
    static const std::size_t componentId;                                       \
    virtual bool isType(const std::size_t &typeId) const override;              \
    virtual size_t getId() override;                                            \
    static FixedArrayAllocator<T, N> fixedArrayAllocator;                       \
    static void destroy(T *ptr);                                                \
    template <typename... Args>                                                 \
    static auto createComponent(Args &&... params)                              \
    {                                                                           \
        auto ptr = fixedArrayAllocator.allocate(std::forward<Args>(params)...); \
        RenderScript::collection.insert(ptr);                                   \
        collection.insert(ptr);                                                 \
        return std::shared_ptr<T>(ptr, &destroy);                               \
    }                                                                           \
    T(Entity *entity, void **) : T(entity){};                                   \
    friend FixedArrayAllocator<T, N>;                                           \
    COMPONENT_EDITABLE_DEC();

#define RENDERSCRIPT_DEFINATION(P, T, N)                                            \
    const std::size_t T::componentId = Component::genComponentId(#T);               \
    size_t T::getId()                                                               \
    {                                                                               \
        return componentId;                                                         \
    }                                                                               \
    bool T::isType(const std::size_t &typeId) const                                 \
    {                                                                               \
        if (typeId == T::componentId)                                               \
            return true;                                                            \
        return P::isType(typeId);                                                   \
    }                                                                               \
    FixedArrayAllocator<T, N> T::fixedArrayAllocator = FixedArrayAllocator<T, N>(); \
                                                                                    \
    void T::destroy(T *ptr)                                                         \
    {                                                                               \
        collection.erase(ptr);                                                      \
        RenderScript::collection.erase(ptr);                                        \
        fixedArrayAllocator.deallocate(ptr);                                        \
    }                                                                               \
    COMPONENT_EDITABLE_DEF(T);
} // namespace WeilanEngine
