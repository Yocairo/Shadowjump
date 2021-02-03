#pragma once

#include <Windows.h>

#include "Action.h"

class OutputSimulator
{
private:
    static bool s_cancellationRequested;
    void mouseButtonAction(INT, bool);
    void keyDown(INT);
    void keyUp(INT);
    void executeAction(SAction &);
public:
    static DWORD CALLBACK performerThread(LPVOID);
    static void requestCancellation();
};