#pragma once

#include <Windows.h>

LRESULT CALLBACK LowLevelKeyboardHookProc(int, WPARAM, LPARAM);
LRESULT CALLBACK LowLevelMouseHookProc(int, WPARAM, LPARAM);