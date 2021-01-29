#include "pch.h"

#include <Windows.h>
#include <string>
#include "CoD4functions.h"

/// <summary>LowLevelKeyboardHookProc is the hKeyboardHook callback procedure</summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">Identifier ot he keyboard message (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP</param>
/// <param name="lParam">Pointer to a KBDLLHOOKSTRUCT structure</param>
LRESULT CALLBACK LowLevelKeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    extern HHOOK hKeyboardHook;
    if (code != HC_ACTION)
    {
        return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
    }

    KBDLLHOOKSTRUCT *pLLKeyboardHookStruct = (KBDLLHOOKSTRUCT *)lParam;

    // Temporary 'proof of concept' that the keyboard hook works properly
    if ((wParam == WM_KEYDOWN) && (pLLKeyboardHookStruct->vkCode == VK_F1))
    {
        iprintln("^1F1 pressed!");
    }

    // TODO: Actual key handling

    return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
}