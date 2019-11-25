#pragma once
#include "../Memory/FixedArrayAllocator.hpp"

#include <functional>
#include <iostream>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <unordered_set>

#define COMPONENT_DECLARATION_NEW(P, T)                            \
public:                                                            \
    static const std::size_t componentId;                          \
    virtual bool isType(const std::size_t &typeId) const override; \
    static std::set<T *> collection;                               \
    template <typename... Args>                                    \
    static auto createComponent(Entity *go, Args &&... params)     \
    {                                                              \
        auto ptr = new T(go, std::forward<Args>(params)...);       \
        collection.insert(ptr);                                    \
        return std::shared_ptr<T>(ptr);                            \
    }                                                              \
    static void destroy(T *ptr);                                   \
    virtual std::size_t getId() override;

#define COMPONENT_DEFINATION_NEW(P, T)                               \
    const std::size_t T::componentId = std::hash<std::string>()(#T); \
    bool T::isType(const std::size_t &typeId) const                  \
    {                                                                \
        if (typeId == T::componentId)                                \
            return true;                                             \
        return P::isType(typeId);                                    \
    }                                                                \
    std::size_t T::getId() { return componentId; }                   \
    std::set<T *> T::collection = std::set<T *>();                   \
    void T::destroy(T *ptr)                                          \
    {                                                                \
        collection.erase(ptr);                                       \
        delete ptr;                                                  \
    }

#define COMPONENT_DECLARATION_ABSTRACT(P, T)                       \
public:                                                            \
    static const std::size_t componentId;                          \
    virtual bool isType(const std::size_t &typeId) const override; \
    virtual std::size_t getId() override;

#define COMPONENT_DEFINATION_ABSTRACT(P, T)                          \
    const std::size_t T::componentId = std::hash<std::string>()(#T); \
    bool T::isType(const std::size_t &typeId) const                  \
    {                                                                \
        if (typeId == T::componentId)                                \
            return true;                                             \
        return P::isType(typeId);                                    \
    }                                                                \
    std::size_t T::getId() { return componentId; }

#define COMPONENT_DECLARATION(P, T, N)                                              \
public:                                                                             \
    static const std::size_t componentId;                                           \
    virtual bool isType(const std::size_t &typeId) const override;                  \
    static std::set<T *> collection;                                                \
    static FixedArrayAllocator<T, N> fixedArrayAllocator;                           \
    static void destroy(T *ptr);                                                    \
    template <typename... Args>                                                     \
    static auto createComponent(Entity *go, Args &&... params)                      \
    {                                                                               \
        auto ptr = fixedArrayAllocator.allocate(go, std::forward<Args>(params)...); \
        collection.insert(ptr);                                                     \
        return std::shared_ptr<T>(ptr, &destroy);                                   \
    }                                                                               \
    virtual std::size_t getId() override;                                           \
    friend FixedArrayAllocator<T, N>;

#define COMPONENT_DEFINATION(P, T, N)                                               \
    const std::size_t T::componentId = std::hash<std::string>()(#T);                \
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
        fixedArrayAllocator.deallocate(ptr);                                        \
    }                                                                               \
    std::size_t T::getId() { return componentId; }                                  \
    std::set<T *> T::collection = std::set<T *>();

#define COMPONENT_EDITABLE_DEC()                                     \
public:                                                              \
    static bool isComponentReg;                                      \
private:

#define COMPONENT_EDITABLE_DEF(T)                                                                                       \
    bool T::isComponentReg = registerComponent<T>([](Entity *go, void **args) { go->AddComponent<T>(args); });

namespace wlEngine
{
class Entity;
struct Component
{
public:
    static std::size_t genComponentId(const std::string &);
    static std::map<std::size_t, std::string> *componentIdToName; // this now is only used for Script, leave it here
                                                                  // for extensibility
    static std::map<std::size_t, std::string> *getComponentIdToName();
    static std::map<std::size_t, std::function<void(Entity *, void **)>> *componentFactoryList;
    static std::map<std::size_t, std::function<void(Entity *, void **)>> *getComponentFactoryList();
    Component(Entity *go);
    Entity *entity = nullptr;

    static const std::size_t componentId;
    virtual bool isType(const std::size_t &typeId) const;

    virtual void destruct(Entity *go){};
    virtual std::size_t getId();
    virtual ~Component(){};
    template <class T>
    static bool registerComponent(std::function<void(Entity *, void**)>);
    bool enable;
};

template <class T>
bool Component::registerComponent(std::function<void(Entity *, void**)> f)
{
    (*getComponentFactoryList())[T::componentId] = f;
    return true;
}
} // namespace wlEngine
