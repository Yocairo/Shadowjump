#pragma once

#include <Windows.h>

class Hooks
{
private:
    static Hooks *s_pInstance;
    HHOOK m_hKeyboardHook;
    HHOOK m_hMouseHook;

public:
    static Hooks *getInstance();
    HHOOK getKeyboardHook();
    HHOOK getMouseHook();
    bool installHooks(HMODULE, DWORD);
    void uninstallHooks();
};