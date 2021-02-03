#include "pch.h"

#include "OutputSimulator.h"
#include "Storage.h"
#include "TimeHelper.h"
#include <iostream>
#include <iomanip>
#include <thread>
#include "CoD4functions.h"
#include "InputHandler.h"
#include "CoD4Application.h"

CViewAngles_t *pViewAnglesAbsMod = (CViewAngles_t *)(0x00400000 + 0x0039E76C);
CViewAngles_t *pViewAnglesTotal = (CViewAngles_t *)(0x00400000 + 0x00884FD8);

/// <summary>
/// Simulate a mouse down or up action of a specific mouse button
/// </summary>
/// <param name="button">0 = left, 1 = right mouse button</param>
/// <param name="down">Whether the button needs to be pushed down (true) or released (false)</param>
void OutputSimulator::mouseButtonAction(INT button, bool down)
{
    POINT point;
    if (GetCursorPos(&point))
    {
        // TODO: Figure out how this works with 2 monitors
        //double fx = point.x * (static_cast<double>(0xFFFF) / GetSystemMetrics(SM_CXSCREEN) - 1);
        //double fy = point.y * (static_cast<double>(0xFFFF) / GetSystemMetrics(SM_CYSCREEN) - 1);

        INPUT input = {0};
        input.type = INPUT_MOUSE;
        if (down)
        {
            input.mi.dwFlags = (button == 0) ? MOUSEEVENTF_LEFTDOWN : MOUSEEVENTF_RIGHTDOWN;
        }
        else
        {
            input.mi.dwFlags = (button == 0) ? MOUSEEVENTF_LEFTUP : MOUSEEVENTF_RIGHTUP;
        }
        input.mi.dx = 0;
        input.mi.dy = 0;

        SendInput(1, &input, sizeof(input));
    }
}

/// <summary>
/// Simulate a key being held
/// </summary>
/// <param name="button">The virtual key code that matches the key</param>
void OutputSimulator::keyDown(INT button)
{
    // TODO: Probably use SendInput due to deprecation
    keybd_event((BYTE)button, 0, 0, 0);
}

/// <summary>
/// Simulate a key being released
/// </summary>
/// <param name="button">The virtual key code that matches the key</param>
void OutputSimulator::keyUp(INT button)
{
    // TODO: Probably use SendInput due to deprecation
    keybd_event((BYTE)button, 0, KEYEVENTF_KEYUP, 0);
}

/// <summary>
/// Execute a specific action
/// </summary>
/// <param name="action">The action to execute</param>
void OutputSimulator::executeAction(SAction &action)
{
    switch (action.type)
    {
        case ActionType::ViewAngles:
        {
            pViewAnglesTotal->yaw += (action.viewAngles.yaw - pViewAnglesAbsMod->yaw);
            pViewAnglesTotal->pitch += (action.viewAngles.pitch - pViewAnglesAbsMod->pitch);
        } break;
        case ActionType::MouseButtonDown:
        {
            mouseButtonAction(action.mouseButtonDown.button, true);
        } break;
        case ActionType::MouseButtonUp:
        {
            mouseButtonAction(action.mouseButtonUp.button, false);
        } break;
        case ActionType::KeyDown:
        {
            keyDown(action.keyUp.button);
        } break;
        case ActionType::KeyUp:
        {
            keyUp(action.keyPress.button);
        } break;
        default:
        {
            cout << "Unknown action " << action.type << endl;
            // This isn't supposed to be reached.
        } break;
    }
}

/// <summary>
/// Start performing the actions in the timed actions map
/// </summary>
DWORD CALLBACK OutputSimulator::performerThread(LPVOID lpParameter)
{
    OutputSimulator *pInst = (OutputSimulator *)lpParameter;
    if (!Storage::s_timedActionsMap.empty())
    {
        multimap<UINT, SAction>::iterator it = Storage::s_timedActionsMap.begin();
        __int64 delay = it->first;
        SAction action = it->second;

        TimeHelper timeHelper {};
        timeHelper.setStartTime();
        while (it != Storage::s_timedActionsMap.end())
        {
            if (timeHelper.getTimePassed() >= delay)
            {
                pInst->executeAction(action);

                // Make sure there is a next action
                if (++it == Storage::s_timedActionsMap.end())
                {
                    break;
                }

                delay = it->first;
                action = it->second;
            }
            
            // If the next action isn't due yet, only then can we sleep
            if (delay > ((ULONG)timeHelper.getTimePassed() + 1))
            {
                Sleep(1);
            }
        }
    }

    CoD4Application::getInstance()->onPlaybackFinished();
    return 0;
}