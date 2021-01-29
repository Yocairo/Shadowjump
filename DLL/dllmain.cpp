// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "Hooks.h"
#include <cstdio>
#include <iostream>
#include <mmsystem.h>

// These are required for PlaySound, which is a nice way to tell if a function has been reached
#pragma comment(lib, "Winmm.lib")
#define ACTIVATE_SOUND      L"C:\\Windows\\Media\\Speech On.wav"
#define DEACTIVATE_SOUND    L"C:\\Windows\\Media\\Speech Off.wav"

typedef struct
{
    HMODULE dllHandle;
} SDllThreadParams;

using namespace std;

/// <summary>
/// Thread that does all the work in this DLL
/// </summary>
/// <param name="lpParameter">Parameters passed to this thread</param>
/// <returns>Always 0</returns>
DWORD CALLBACK DllThread(LPVOID lpParameter)
{
    SDllThreadParams *pParams = (SDllThreadParams *)lpParameter;
    HMODULE dllHandle = pParams->dllHandle;
    delete pParams; // Caller is not able to free this memory, so we must do so

    // Allocate and redirect console so we can display debug output
    FILE *pNewConsole;
    AllocConsole();
    freopen_s(&pNewConsole, "CONOUT$", "w", stdout);
    freopen_s(&pNewConsole, "CONOUT$", "w", stderr);
    cout.clear();
    cerr.clear();

    cout << "Attempting to install hooks.." << endl;

    // Try to install the low level hook(s)
    if (installHooks(dllHandle, GetCurrentThreadId()))
    {
        // We successfully installed the hook(s)
        cout << "Successfully installed hooks" << endl;

        MSG uMsg;
        while (GetMessage(&uMsg, NULL, 0, 0))
        {
            TranslateMessage(&uMsg);
            DispatchMessage(&uMsg);
        }

        uninstallHooks();
    }

    // Leaving the thread, play a sound before we go
    cout << "Failed to install hooks" << endl;
    return 0;
}

/// <summary>
/// Main entry point of DLL
/// </summary>
/// <param name="hModule"></param>
/// <param name="dwReasonForCall"></param>
/// <param name="lpReserved"></param>
/// <returns></returns>
BOOL WINAPI DllMain(HMODULE hModule, DWORD dwReasonForCall, LPVOID lpReserved)
{
    static HANDLE threadHandle = NULL;
    switch (dwReasonForCall)
    {
        case DLL_PROCESS_ATTACH:
        {
            // Prepare arguments we will be passing to new thread
            SDllThreadParams *pParams = new SDllThreadParams;
            pParams->dllHandle = hModule; // Handle to "this" DLL

            // DLL was attached to a process. Since we injected it specifically into iw3mp.exe, we can now simply install a low level keyboard hook
            // This needs to be done in a new thread, since DllMain mustn't do more than simple initialization (or thread creation)
            threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DllThread, pParams, 0, NULL);

        } break;
        case DLL_THREAD_ATTACH:
        {

        } break;
        case DLL_THREAD_DETACH:
        {

        } break;
        case DLL_PROCESS_DETACH:
        {
            if (threadHandle != NULL)
            {
                // We are disabling this warning, because there is no way we can WaitForSingleObject when the process is detaching
                // This means TerminateThread is really the only option we have here
#pragma warning(disable : 6258)
                (void)TerminateThread(threadHandle, 0);
            }
        } break;
    }
    return true;
}

