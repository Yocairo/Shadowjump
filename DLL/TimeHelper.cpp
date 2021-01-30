#include "pch.h"

#include <chrono>

using namespace std;

static chrono::steady_clock::time_point startPoint;

/// <summary>
/// Set the start timepoint of the recording
/// </summary>
void setStartTime()
{
    startPoint = chrono::steady_clock::now();
}

/// <summary>
/// Get time passed since the start of the recording
/// </summary>
/// <returns>Time in ms</returns>
UINT getTimePassed()
{
    // Cast to UINT is OK because we definitely don't support more than 49.7 days of recording
    return (UINT)chrono::duration_cast<chrono::milliseconds>(chrono::steady_clock::now() - startPoint).count();
}