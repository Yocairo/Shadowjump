#include "pch.h"

#include <Windows.h>
#include "KeyboardHandler.h"
#include <iostream>

HHOOK hKeyboardHook = NULL;

using namespace std;

/// <summary>
/// Install the necessary hook(s)
/// </summary>
/// <param name="dllHandle">Handle to the DLL</param>
/// <param name="threadId">Thread id for the hook</param>
/// <returns>true if successfully installed</returns>
bool installHooks(HMODULE dllHandle, DWORD threadId)
{
    // For low level hook, we can't use thread id
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardHookProc, dllHandle, 0);
    if (hKeyboardHook == NULL)
    {
        cout << "SetWindowsHookEx error code: " << GetLastError() << endl;
        return false;
    }

    return true;
}

/// <summary>
/// Uninstall any hooks installed by a call to installHooks
/// </summary>
void uninstallHooks()
{
    if (hKeyboardHook)
    {
        UnhookWindowsHookEx(hKeyboardHook);
    }

    hKeyboardHook = NULL;
}