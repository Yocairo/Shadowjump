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
    void perform();
public:
    void playback();
};