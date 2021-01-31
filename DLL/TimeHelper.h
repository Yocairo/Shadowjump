#pragma once

#include <Windows.h>
#include <chrono>

class TimeHelper
{
private:
    std::chrono::steady_clock::time_point m_startPoint;
public:
    static bool improveTimerResolution();
    static void resetTimerResolution();
    void setStartTime();
    UINT getTimePassed();
};