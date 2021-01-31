#include "pch.h"

#include <Windows.h>
#include <iostream>
#include "InputHandler.h"
#include "Hooks.h"

using namespace std;

Hooks *Hooks::s_pInstance = nullptr;

/// <summary>
/// Get the instance of Hooks singleton
/// </summary>
/// <returns></returns>
Hooks *Hooks::getInstance()
{
    if (s_pInstance == nullptr)
    {
        s_pInstance = new Hooks();
    }

    return s_pInstance;
}

/// <summary>
/// Get a handle to the keyboard hook
/// </summary>
/// <returns></returns>
HHOOK Hooks::getKeyboardHook()
{
    return m_hKeyboardHook;
}

/// <summary>
/// Get a handle to the mouse hook
/// </summary>
/// <returns></returns>
HHOOK Hooks::getMouseHook()
{
    return m_hMouseHook;
}

/// <summary>
/// Install the necessary hook(s)
/// </summary>
/// <param name="dllHandle">Handle to the DLL</param>
/// <param name="threadId">Thread id for the hook</param>
/// <returns>true if successfully installed</returns>
bool Hooks::install(HMODULE dllHandle, DWORD threadId)
{
    // For low level hook, we can't use thread id
    m_hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)LowLevelKeyboardHookProc, dllHandle, threadId);
    m_hMouseHook = SetWindowsHookEx(WH_MOUSE_LL, (HOOKPROC)LowLevelMouseHookProc, dllHandle, threadId);
    if ((m_hKeyboardHook == NULL) || (m_hMouseHook == NULL))
    {
        cout << "Hook error code: " << GetLastError() << endl;
        return false;
    }

    return true;
}

/// <summary>
/// Uninstall any hooks installed by a call to installHooks
/// </summary>
void Hooks::uninstall()
{
    if (m_hKeyboardHook)
    {
        (void)UnhookWindowsHookEx(m_hKeyboardHook);
        m_hKeyboardHook = NULL;
    }

    if (m_hMouseHook)
    {
        (void)UnhookWindowsHookEx(m_hMouseHook);
        m_hMouseHook = NULL;
    }
}