#pragma once
namespace WeilanEngine
{
struct Rect
{
    int x;
    int y;
    int width;
    int height;
    Rect() : x(0), y(0), width(0), height(0){};
    Rect(const int &x, const int &y, const int &width, const int &height) : x(x), y(y), width(width), height(height){};
    Rect(const Rect &other) : x(other.x), y(other.y), width(other.width), height(other.height)
    {
    }
    const Rect &operator=(const Rect &other)
    {
        x = other.x;
        y = other.y;
        width = other.width;
        height = other.height;
        return *this;
    }
};
/* Animation stores information that a gamobject will use to clip the sprint sheet*/
struct Frame
{
    float duration;
    Rect rect;
    Frame(const Rect &rect, const float &d) : duration(d), rect(rect) {}
    Frame() : duration(), rect() {}
};
} // namespace WeilanEngine
