// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <cstdio>
#include <iostream>
#include "Hooks.h"

static FILE *pNewConsoleOut = nullptr;
static FILE *pNewConsoleErr = nullptr;
static HMODULE dllHandle = nullptr;

using namespace std;

/// <summary>
/// Commit sudoku
/// </summary>
void unloadCurrentDll()
{
    uninstallHooks();

    if (pNewConsoleOut)
    {
        fclose(pNewConsoleOut);
    }

    if (pNewConsoleErr)
    {
        fclose(pNewConsoleErr);
    }

    FreeConsole();

    FreeLibraryAndExitThread(dllHandle, 0);
}

/// <summary>
/// Thread that does all the work in this DLL
/// </summary>
/// <param name="lpParameter">Parameters passed to this thread</param>
/// <returns>Always 0</returns>
DWORD CALLBACK DllThread(LPVOID lpParameter)
{
    // Allocate and redirect console so we can display debug output
    AllocConsole();
    freopen_s(&pNewConsoleOut, "CONOUT$", "w", stdout);
    freopen_s(&pNewConsoleErr, "CONOUT$", "w", stderr);
    cout.clear();
    cerr.clear();

    cout << "Attempting to install hooks.." << endl;

    // Try to install the low level hook(s)
    if (installHooks(dllHandle, 0))
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
    else
    {
        cout << "Failed to install hooks" << endl;
    }

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
            dllHandle = hModule; // Handle to "this" DLL

            // DLL was attached to a process. Since we injected it specifically into iw3mp.exe, we can now simply install a low level keyboard hook
            // This needs to be done in a new thread, since DllMain mustn't do more than simple initialization (or thread creation)
            threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)DllThread, nullptr, 0, nullptr);

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

