#pragma once

#include "TimeHelper.h"
#include "InputHandler.h"
#include "OutputSimulator.h"

class CoD4Application
{
private:
    static DWORD CALLBACK sampleViewAngleThread(LPVOID);
    static CoD4Application *s_pInstance;
    TimeHelper *m_pTimeHelper;
    InputHandler *m_pInputHandler;
    OutputSimulator *m_pOutputSimulator;
    HMODULE m_dllInstance;
    FILE *m_pNewConsoleOut;
    FILE *m_pNewConsoleErr;
    bool m_isRecording;
    bool m_isPlayingBack;
    HANDLE m_samplerHandle;
    HANDLE m_performerHandle;
public:
    static CoD4Application *getInstance(HMODULE);
    static CoD4Application *getInstance(); // Only callable after instance has been created with HMODULE argument
    TimeHelper *getTimeHelper();
    void run();
    void onPlaybackFinished(bool);
    bool isFunctionKey(UINT keyCode);
    void handleFunctionKey(WPARAM, UINT);
    bool isRecording();
    void exit();
private:
    CoD4Application(HMODULE);
    ~CoD4Application();
};