#pragma once

enum ActionType
{
    Unknown = 0,
    MouseMove,
    MouseButtonDown,
    MouseButtonUp,
    KeyDown,
    KeyUp,
};

typedef struct
{
    INT x : 16;
    INT y : 16;
} SMouseMove;

typedef struct
{
    INT button;
} SMouseDown;

typedef struct
{
    INT button;
} SMouseUp;

typedef struct
{
    INT button;
} SMouseClick;

typedef struct
{
    INT button;
} SKeyDown;

typedef struct
{
    INT button;
} SKeyUp;

typedef struct
{
    INT button;
} SKeyPress;

typedef struct
{
    ActionType type;
    union
    {
        SMouseMove mouseMove;
        SMouseDown mouseButtonDown;
        SMouseUp mouseButtonUp;
        SMouseClick mouseClick;
        SKeyDown keyDown;
        SKeyUp keyUp;
        SKeyPress keyPress;
        INT rawValue;
    };
} SAction;

class Action
{
public:
    static DWORD s_prevKeyDownButton;
    static INT s_prevXCoord;
    static INT s_prevYCoord;
public:
    static void resetInputStates();
    static void storeMouseAction(UINT, WPARAM, LPARAM);
    static void storeKeyAction(UINT, WPARAM, LPARAM);
private:
    Action();
};