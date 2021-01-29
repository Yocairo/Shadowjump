#include "pch.h"

#include <Windows.h>
#include <string>
#include "CoD4functions.h"

/// <summary>The system calls this function every time a new keyboard input event is about to be posted into a thread input queue</summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">Identifier of the keyboard message (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP)</param>
/// <param name="lParam">Pointer to a KBDLLHOOKSTRUCT structure</param>
LRESULT CALLBACK LowLevelKeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    extern HHOOK hKeyboardHook;
    if (code != HC_ACTION)
    {
        return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
    }

    KBDLLHOOKSTRUCT *pLLKeyboardHookStruct = (KBDLLHOOKSTRUCT *)lParam;

    // TODO: Actual key handling

    return 0;
}

/// <summary>The system calls this function every time a new mouse input event is about to be posted into a thread input queue</summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">Identifier of the mouse message (WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MOUSEMOVE, WM_MOUSEWHEEL, WM_MOUSEHWHEEL, WM_RBUTTONDOWN, WM_RBUTTONUP)</param>
/// <param name="lParam">Pointer to a KBDLLHOOKSTRUCT structure</param>
LRESULT CALLBACK LowLevelMouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    extern HHOOK hMouseHook;
    if (code != HC_ACTION)
    {
        return CallNextHookEx(hMouseHook, code, wParam, lParam);
    }

    MSLLHOOKSTRUCT *pLLMouseHookStruct = (MSLLHOOKSTRUCT *)lParam;

    // TODO: Actual mouse handling

    return 0;
}