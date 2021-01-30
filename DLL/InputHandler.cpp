#include "pch.h"

#include <Windows.h>
#include <string>
#include <chrono>
#include "CoD4functions.h"
#include "Action.h"
#include "TimeHelper.h"
#include <iostream>
#include "Storage.h"

using namespace std;

static bool isRecording = false;
static bool isPlayingBack = false;

/// <summary>
/// Check if a key code is a function key for this application
/// </summary>
/// <param name="keyCode">The key code to check</param>
/// <returns>true if key code is a function key and handleFunctionKey should be called</returns>
bool isFunctionKey(UINT keyCode)
{
    // F1 = start recording
    // F2 = stop recording
    // F3 = start playback
    // F4 = stop playback
    // F5 = unload DLL
    if ((keyCode >= VK_F1) && (keyCode <= VK_F5))
    {
        return true;
    }

    return false;
}

/// <summary>
/// Handle a function key action
/// </summary>
/// <param name="wParam">The action (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP)</param>
/// <param name="keyCode">The virtual key code (VK_F1, etc)</param>
void handleFunctionKey(WPARAM wParam, UINT keyCode)
{
    if (keyCode == VK_F5) // Unload DLL key
    {
        cout << "Unloading DLL" << endl;
        extern void unloadCurrentDll(); // dllmain.cpp
        unloadCurrentDll(); // :-(
        return;
    }
    else if (keyCode == VK_F1) // Start recording
    {
        if (!isRecording)
        {
            Storage::getInstance()->clear();
            resetKeyStates();
            setStartTime();
            isRecording = true;
            iprintln("^2Started ^7recording");
        }
    }
    else if (keyCode == VK_F2) // Stop recording
    {
        if (isRecording)
        {
            isRecording = false;
            Storage::getInstance()->flush();
            iprintln("^1Stopped ^7recording");
        }
    }
    else if (keyCode == VK_F3) // Start playback
    {
        if (!isPlayingBack)
        {
            isPlayingBack = true;
            iprintln("^5Started ^7playback");
        }
    }
    else if (keyCode == VK_F4) // Stop playback
    {
        if (isPlayingBack)
        {
            isPlayingBack = false;
            iprintln("^Stopped ^7playback");
        }
    }
    else
    {
        cout << keyCode << " is not a function key" << endl;
    }
}

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

    // If the input is a function key, handle it. If it's not and we're recording, store the action
    KBDLLHOOKSTRUCT *pLLKeyboardStruct = (KBDLLHOOKSTRUCT *)lParam;
    if (isFunctionKey(pLLKeyboardStruct->vkCode))
    {
        handleFunctionKey(wParam, pLLKeyboardStruct->vkCode);
    }
    else if (isRecording)
    {
        Action::storeKeyAction(getTimePassed(), wParam, lParam);
    }
    else
    {
        return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
    }

    return 0;
}

/// <summary>The system calls this function every time a new mouse input event is about to be posted into a thread input queue</summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">Identifier of the mouse message (WM_LBUTTONDOWN, WM_LBUTTONUP, WM_MOUSEMOVE, WM_MOUSEWHEEL, WM_MOUSEHWHEEL, WM_RBUTTONDOWN, WM_RBUTTONUP)</param>
/// <param name="lParam">Pointer to a KBDLLHOOKSTRUCT structure</param>
LRESULT CALLBACK LowLevelMouseHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    extern HHOOK hMouseHook;
    if ((code != HC_ACTION) || !isRecording)
    {
        return CallNextHookEx(hMouseHook, code, wParam, lParam);
    }

    // If we're recording, store the action
    Action::storeMouseAction(getTimePassed(), wParam, lParam);

    return 0;
}