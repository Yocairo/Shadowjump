#pragma once

#include <Windows.h>

#define VK_START_RECORDING  VK_F1
#define VK_STOP_RECORDING   VK_F2
#define VK_START_PLAYBACK   VK_F3
#define VK_STOP_PLAYBACK    VK_F4

typedef struct
{
    HMODULE dllHandle;
} SDllThreadParams;

void stopRecording();
void stopPlayback();

DWORD CALLBACK ApplicationThread(LPVOID);
