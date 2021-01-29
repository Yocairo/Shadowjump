#include "pch.h"

#include <Windows.h>
#include <iostream>
#include "InputHandler.h"

HHOOK hKeyboardHook = NULL;
HHOOK hMouseHook = NULL;

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
    hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardHookProc, dllHandle, threadId);
    hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseHookProc, dllHandle, threadId);
    if ((hKeyboardHook == NULL) || (hMouseHook == NULL))
    {
        cout << "Hook error code: " << GetLastError() << endl;
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
        (void)UnhookWindowsHookEx(hKeyboardHook);
        hKeyboardHook = NULL;
    }

    if (hMouseHook)
    {
        (void)UnhookWindowsHookEx(hMouseHook);
        hMouseHook = NULL;
    }
}