#pragma once
#include "Component.hpp"

#define SCRIPT_DECLARATION(P, T, N)                                             \
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
        Script::collection.insert(ptr);                                         \
        collection.insert(ptr);                                                 \
        return std::shared_ptr<T>(ptr, &destroy);                               \
    }                                                                           \
    T(Entity *entity, void **) : T(entity){};                                   \
    friend FixedArrayAllocator<T, N>;                                           \
    COMPONENT_EDITABLE_DEC();

#define SCRIPT_DEFINATION(P, T, N)                                                  \
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
        Script::collection.erase(ptr);                                              \
        fixedArrayAllocator.deallocate(ptr);                                        \
    }                                                                               \
    COMPONENT_EDITABLE_DEF(T);

namespace wlEngine
{
struct Script : public Component
{
    static const std::size_t componentId;
    virtual size_t getId() override;
    virtual bool isType(const std::size_t &typeId) const override;
    static std::set<Script *> collection;
    Script(Entity *entity) : Component(entity){};
    virtual void update() = 0;
    virtual void postInit(){};
};
} // namespace wlEngine
