#include "pch.h"
#include "Action.h"
#include "Storage.h"
#include <iostream>
#include "CoD4Application.h"

// For key up actions, Windows spams these if key is pressed down.
// It's unnecessary to store them all, we can just store the down and (if available) up action
DWORD Action::s_prevKeyDownButton = -1;
INT Action::s_prevXCoord = 0;
INT Action::s_prevYCoord = 0;

/// <summary>
/// Reset key state to before recording
/// </summary>
void Action::resetInputStates()
{
    // TODO: Perhaps move to InputHandler.cpp
    // TODO: Reset complete keyboard
    s_prevKeyDownButton = -1;
}

void Action::storeCurrentViewAngles()
{
    UINT ts = CoD4Application::getInstance()->getTimeHelper()->getTimePassed();
    SAction action;
    action.type = ActionType::ViewAngles;
    extern CViewAngles_t *pViewAnglesAbsMod;
    action.viewAngles.yaw = pViewAnglesAbsMod->yaw;
    action.viewAngles.pitch = pViewAnglesAbsMod->pitch;

    Storage::getInstance()->add(ts, &action);
}

/// <summary>
/// Store an action that was performed with the mouse
/// </summary>
/// <param name="ts">Time offset compared to start of recording</param>
/// <param name="wParam">wParam from mouse hook function</param>
/// <param name="lParam">lParam from mouse hook function</param>
void Action::storeMouseAction(UINT ts, WPARAM wParam, LPARAM lParam)
{
    MSLLHOOKSTRUCT *pLLMouseHookStruct = (MSLLHOOKSTRUCT *)lParam;
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
            action.type = ActionType::MouseButtonUp;
            action.mouseButtonUp.button = (wParam == WM_RBUTTONUP) ? 1 : 0; // Left is 0, right is 1
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
void Action::storeKeyAction(UINT ts, WPARAM wParam, LPARAM lParam)
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
        case WM_SYSKEYDOWN:
        {
            if (s_prevKeyDownButton != pLLKeyboardHookStruct->vkCode)
            {
                action.type = ActionType::KeyDown;
                action.keyDown.button = pLLKeyboardHookStruct->vkCode;
                s_prevKeyDownButton = pLLKeyboardHookStruct->vkCode;
            }
        } break;
        case WM_KEYUP:
        case WM_SYSKEYUP:
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
