#pragma once

#include <Windows.h>
#include "TimeHelper.h"

class InputHandler
{
private:
    TimeHelper *m_pTimeHelper;
    static InputHandler *s_pInputHandler;

public:
    static InputHandler *getInstance();

private:
    InputHandler();
    ~InputHandler();
    bool isFunctionKey(UINT);
    void handleFunctionKey(WPARAM, UINT);
};

LRESULT CALLBACK LowLevelKeyboardHookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelMouseHookProc(int, WPARAM, LPARAM);