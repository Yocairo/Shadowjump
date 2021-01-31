#include "pch.h"

#include <chrono>
#include <timeapi.h>
#include "TimeHelper.h"

#pragma comment (lib, "Winmm.lib") // timeapi library

using namespace std;

/// <summary>
/// Improve Windows timer resolution to 1 ms
/// </summary>
bool TimeHelper::improveTimerResolution()
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
/// Reset the previously improved timer resolution
/// </summary>
void TimeHelper::resetTimerResolution()
{
	(void)timeEndPeriod(1);
}

/// <summary>
/// Set the start timepoint of the recording
/// </summary>
void TimeHelper::setStartTime()
{
    m_startPoint = chrono::steady_clock::now();
}

/// <summary>
/// Get time passed since the start of the recording
/// </summary>
/// <returns>Time in ms</returns>
UINT TimeHelper::getTimePassed()
{
    // Cast to UINT is OK because we definitely don't support more than 49.7 days of recording
    return (UINT)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - m_startPoint).count();
}