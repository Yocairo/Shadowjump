#pragma once

enum ActionType
{
    Unknown = 0,
    ViewAngles,
    MouseButtonDown,
    MouseButtonUp,
    KeyDown,
    KeyUp,
};

typedef struct
{
    float yaw;
    float pitch;
} SViewAngles;

typedef struct
{
    float pitch;
    float yaw;
} CViewAngles_t;

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
        SViewAngles viewAngles;
        SMouseDown mouseButtonDown;
        SMouseUp mouseButtonUp;
        SMouseClick mouseClick;
        SKeyDown keyDown;
        SKeyUp keyUp;
        SKeyPress keyPress;
        ULONGLONG rawValue;
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
    static void storeCurrentViewAngles();
private:
    Action();
};