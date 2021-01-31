#include "pch.h"

#include "InputHandler.h"
#include <Windows.h>
#include <string>
#include <chrono>
#include "CoD4functions.h"
#include "Action.h"
#include "TimeHelper.h"
#include <iostream>
#include "Storage.h"
#include "CoD4Application.h"
#include "Hooks.h"

using namespace std;

InputHandler *InputHandler::s_pInputHandler = nullptr;

/// <summary>
/// Get an instance of Singleton InputHandler
/// </summary>
/// <returns></returns>
InputHandler *InputHandler::getInstance()
{
    if (s_pInputHandler == nullptr)
    {
        s_pInputHandler = new InputHandler();
    }

    return s_pInputHandler;
}

InputHandler::InputHandler()
{
    m_pTimeHelper = new TimeHelper();
}

InputHandler::~InputHandler()
{
    delete m_pTimeHelper;
}

// Hooks and their variables

static CoD4Application *pApplication = nullptr;

/// <summary>The system calls this function every time a new keyboard input event is about to be posted into a thread input queue</summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">Identifier of the keyboard message (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP)</param>
/// <param name="lParam">Pointer to a KBDLLHOOKSTRUCT structure</param>
LRESULT CALLBACK LowLevelKeyboardHookProc(int code, WPARAM wParam, LPARAM lParam)
{
    HHOOK hKeyboardHook = Hooks::getInstance()->getKeyboardHook();
    if (code != HC_ACTION)
    {
        return CallNextHookEx(hKeyboardHook, code, wParam, lParam);
    }

    // If the input is a function key, handle it. If it's not and we're recording, store the action
    KBDLLHOOKSTRUCT *pLLKeyboardStruct = (KBDLLHOOKSTRUCT *)lParam;
    if (CoD4Application::getInstance()->isFunctionKey(pLLKeyboardStruct->vkCode))
    {
        CoD4Application::getInstance()->handleFunctionKey(wParam, pLLKeyboardStruct->vkCode);
    }
    else if (CoD4Application::getInstance()->isRecording())
    {
        Action::storeKeyAction(CoD4Application::getInstance()->getTimeHelper()->getTimePassed(), wParam, lParam);
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
    HHOOK hMouseHook = Hooks::getInstance()->getMouseHook();
    if ((code != HC_ACTION) || !CoD4Application::getInstance()->isRecording())
    {
        return CallNextHookEx(hMouseHook, code, wParam, lParam);
    }

    // If we're recording, store the action
    Action::storeMouseAction(CoD4Application::getInstance()->getTimeHelper()->getTimePassed(), wParam, lParam);

    return 0;
}