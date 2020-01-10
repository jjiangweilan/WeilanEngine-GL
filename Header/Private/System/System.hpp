#pragma once
#include <unordered_set>

/* How to Use*/
/* You only need to provide System's defination and update function*/

#define SYSTEM_DECLARATION(SystemName) \
public:                                \
    static SystemName *get();          \
    static SystemName *init();         \
    void update() override;            \
                                       \
private:                               \
    static SystemName *system;         \
    SystemName();

#define SYSTEM_DEFINATION(SystemName)         \
    SystemName *SystemName::system = nullptr; \
    SystemName *SystemName::get()             \
    {                                         \
        return system;                        \
    }                                         \
    SystemName *SystemName::init()            \
    {                                         \
        system = new SystemName();            \
        System::registerSystem(system);       \
        return system;                        \
    }

namespace KuangyeEngine
{
class System
{
public:
    virtual void update(){};
    virtual void PostInit(){};
    static std::unordered_set<System *> collection;

protected:
    static void registerSystem(System *); //register the system to collection
};
} // namespace KuangyeEngine
