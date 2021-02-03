#pragma once

#include <Windows.h>

#include "Action.h"

class OutputSimulator
{
private:
    void mouseButtonAction(INT, bool);
    void keyDown(INT);
    void keyUp(INT);
    void executeAction(SAction &);
public:
    static DWORD CALLBACK performerThread(LPVOID);
};