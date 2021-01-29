// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <cstdio>
#include <iostream>
#include "Application.h"
#include <TlHelp32.h>

/// <summary>
/// Find the id of a process by its name. Returns MAXDWORD if not found
/// </summary>
/// <param name="processName">The name of the process for which to find pid</param>
/// <returns>The pid if found, otherwise MAXDWORD</returns>
DWORD getProcessIdByName(std::string processName)
{
    DWORD processId = MAXDWORD;

    // Create a buffer for the snapshot entries to be stored in
    PROCESSENTRY32 entry;
    entry.dwSize = sizeof(PROCESSENTRY32);

    // Create a snapshot of all current processes
    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

    // Loop through the processes until we find what we want
    while (Process32Next(snapshot, &entry) == TRUE)
    {
        if (_wcsicmp(entry.szExeFile, L"iw3mp.exe") == 0)
        {
            // Found the id of the desired process
            processId = entry.th32ProcessID;
            break;
        }
    }

    CloseHandle(snapshot);
    return processId;
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
            // Ensure we only run our application inside iw3mp.exe
            if (GetCurrentProcessId() != getProcessIdByName("iw3mp.exe"))
            {
                return false;
            }

            // Prepare arguments we will be passing to new thread
            SDllThreadParams *pParams = new SDllThreadParams;
            pParams->dllHandle = hModule; // Handle to "this" DLL

            // DLL was attached to a process. Since we injected it specifically into iw3mp.exe, we can now simply install a low level keyboard hook
            // This needs to be done in a new thread, since DllMain mustn't do more than simple initialization (or thread creation)
            threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)ApplicationThread, pParams, 0, NULL);

        } break;
        case DLL_THREAD_ATTACH:
        {

        } break;
        case DLL_THREAD_DETACH:
        {

        } break;
        case DLL_PROCESS_DETACH:
        {

        } break;
    }
    return true;
}

