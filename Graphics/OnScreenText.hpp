#pragma once
#include "Component/Text.hpp"
namespace wlEngine
{
class OnScreenText
{
public:
    static void init();
    static void print(const std::string &);

private:
    static Entity* m_entity;
    static std::vector<Text> m_texts;
};
} // namespace wlEngine