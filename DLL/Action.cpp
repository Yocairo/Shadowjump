#include "pch.h"
#include "Action.h"
#include "Storage.h"
#include <iostream>

// For key up actions, Windows spams these if key is pressed down.
// It's unnecessary to store them all, we can just store the down and (if available) up action
static DWORD prevKeyDownButton = -1;

/// <summary>
/// Reset key state to before recording
/// </summary>
void resetKeyStates()
{
    // TODO: Perhaps move to InputHandler.cpp
    // TODO: Reset complete keyboard
    prevKeyDownButton = -1;
}

/// <summary>
/// Store an action that was performed with the mouse
/// </summary>
/// <param name="ts">Time offset compared to start of recording</param>
/// <param name="wParam">wParam from mouse hook function</param>
/// <param name="lParam">lParam from mouse hook function</param>
void Action::storeMouseAction(ULONGLONG ts, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT *pLLMouseHookStruct = (MSLLHOOKSTRUCT *)lParam; // TODO: Perhaps use timestamp from this struct
    // Ignore injected messages
    if ((pLLMouseHookStruct->flags & (0x01 | 0x02)) > 0) // TODO: Do we actually want to do this?
    {
        cout << "Ignoring injected mouse action" << endl;
        return;
    }

    SAction action;
    action.type = ActionType::Unknown;
    switch (wParam)
    {
        case WM_LBUTTONDOWN:
        case WM_RBUTTONDOWN:
        {
            action.type = ActionType::MouseButtonDown;
            action.mouseButtonDown.button = (wParam == WM_RBUTTONDOWN) ? 1 : 0; // Left is 0, right is 1
        } break;

        case WM_LBUTTONUP:
        case WM_RBUTTONUP:
        {
            action.type = ActionType::MouseButtonDown;
            action.mouseButtonUp.button = (wParam == WM_RBUTTONDOWN) ? 1 : 0; // Left is 0, right is 1
        } break;

        case WM_MOUSEMOVE:
        {
            action.type = ActionType::MouseButtonDown;
            action.mouseMove.x = pLLMouseHookStruct->pt.x; // TODO: we can make this based on percentage of screen, so it will work in all resolutions
            action.mouseMove.y = pLLMouseHookStruct->pt.y;
        } break;

        case WM_MOUSEWHEEL:
        case WM_MOUSEHWHEEL:
        {
            // TODO: Implement this. Not high priority for now (sorry scroll wheel users / bunny hoppers)
        } break;

        default:
        {
            cout << "Unknown mouse action" << endl;
        } break;
    }

    // Only add the action if there actually is one
    if (action.type != ActionType::Unknown)
    {
        Storage::getInstance()->add(ts, &action);
    }
}

/// <summary>
/// Store an action that was performed with the keyboard
/// </summary>
/// <param name="ts">Time offset compared to start of recording</param>
/// <param name="wParam">wParam from keyboard hook function</param>
/// <param name="lParam">lParam from keyboard hook function</param>
void Action::storeKeyAction(ULONGLONG ts, WPARAM wParam, LPARAM lParam)
{
    KBDLLHOOKSTRUCT *pLLKeyboardHookStruct = (KBDLLHOOKSTRUCT *)lParam; // TODO: Perhaps use timestamp from this struct
    // Ignore injected messages
    if ((pLLKeyboardHookStruct->flags & (0x10 | 0x02)) > 0) // TODO: Do we actually want to do this?
    {
        cout << "Ignoring injected keyboard action" << endl;
        return;
    }

    SAction action;
    action.type = ActionType::Unknown;
    switch (wParam)
    {
        case WM_KEYDOWN:
        //case WM_SYSKEYDOWN:
        {
            if (prevKeyDownButton != pLLKeyboardHookStruct->vkCode)
            {
                action.type = ActionType::KeyDown;
                action.keyDown.button = pLLKeyboardHookStruct->vkCode;
                prevKeyDownButton = pLLKeyboardHookStruct->vkCode;
            }
        } break;
        case WM_KEYUP:
        //case WM_SYSKEYUP:
        {
            action.type = ActionType::KeyUp;
            action.keyUp.button = pLLKeyboardHookStruct->vkCode;
        } break;

        default:
        {
            cout << "Unknown keyboard action" << endl;
        } break;
    }

    // Only add the action if there actually is one
    if (action.type != ActionType::Unknown)
    {
        Storage::getInstance()->add(ts, &action);
    }
}
