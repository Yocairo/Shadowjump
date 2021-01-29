#include "pch.h"

#include <Windows.h>
#include <timeapi.h>

#pragma comment(lib, "Winmm.lib")

/// <summary>
/// Attempt to improve the timer resolution to 1 ms
/// </summary>
/// <returns>true if successfully improved, otherwise false</returns>
bool improveTimerResolution()
{
    TIMECAPS timecaps = {0};
    if (timeGetDevCaps(&timecaps, sizeof(timecaps)) == MMSYSERR_NOERROR)
    {
        if (timecaps.wPeriodMin == 1)
        {
            return (timeBeginPeriod(1) == MMSYSERR_NOERROR);
        }
    }

    return false;
}

/// <summary>
/// Reset the timer resolution back to the default
/// </summary>
void resetTimerResolution()
{
    (void)timeEndPeriod(1); // Can't do anything about it if this fails
}