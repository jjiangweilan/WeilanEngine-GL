#include "Global/Time.hpp"
#include <iostream>
namespace wlEngine
{
float Time::deltaTime = 0.0000001;
double Time::timeAfterGameStart = 0;
float Time::nonScaledDeltaTime = 0;
const float Time::dayDuration = 2700;
float Time::dayTimeInSecond = Time::dayDuration + 1;
float Time::timeScale = 1;

std::chrono::time_point<std::chrono::steady_clock> Time::oldTime = std::chrono::steady_clock::now();
void Time::update()
{
    //delta time
    auto newTime = std::chrono::steady_clock::now();
    deltaTime = std::chrono::duration<float>(newTime - oldTime).count();
    nonScaledDeltaTime = deltaTime;
    deltaTime *= timeScale;
    timeAfterGameStart += deltaTime;
    oldTime = newTime;

    //day time
    dayTimeInSecond += deltaTime;
}

void Time::resetDayTime()
{
    dayTimeInSecond = 0;
}
} // namespace wlEngine
