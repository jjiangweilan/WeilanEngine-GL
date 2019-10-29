#ifndef TIME_H
#define TIME_H
#include <chrono>
namespace wlEngine {
    class Time {
        public:
            static float deltaTime;
            static float nonScaledDeltaTime;

            static float dayTimeInSecond;

            static double timeAfterGameStart;
            static void update();
            static void resetDayTime();
            static const float dayDuration; // the duration of a time period in second (morning + evening + night)
            static void setTimeScale(const float& s);
            static float timeScale;
        private:
            static std::chrono::time_point<std::chrono::steady_clock> oldTime;
    };
}

#endif
