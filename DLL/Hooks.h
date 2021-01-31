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
    bool install(HMODULE, DWORD);
    HHOOK getKeyboardHook();
    HHOOK getMouseHook();
    void uninstall();
};