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
    DWORD x : 16;
    DWORD y : 16;
} SMouseMove;

typedef struct
{
    DWORD button;
} SMouseDown;

typedef struct
{
    DWORD button;
} SMouseUp;

typedef struct
{
    DWORD button;
} SMouseClick;

typedef struct
{
    DWORD button;
} SKeyDown;

typedef struct
{
    DWORD button;
} SKeyUp;

typedef struct
{
    DWORD button;
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
        DWORD rawValue;
    };
} SAction;

void resetKeyStates();

class Action
{
public:
    static void storeMouseAction(ULONGLONG, WPARAM, LPARAM);
    static void storeKeyAction(ULONGLONG, WPARAM, LPARAM);
private:
    Action();
};