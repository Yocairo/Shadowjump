#include "pch.h"

#include <string>

typedef void (*SV_GameSendServerCommand_t)(int, const char *, const char *);
typedef void (*Dvar_FindMalleableVar_t)(const char *);

static SV_GameSendServerCommand_t SV_GameSendServerCommand = (SV_GameSendServerCommand_t)0x00531320;
static Dvar_FindMalleableVar_t Dvar_FindMalleableVar = (Dvar_FindMalleableVar_t)0x569DA0;

/// <summary>
/// Print a message on bottom left of screen by calling SV_GameSendServerCommand
/// </summary>
/// <param name="strMsg">The message that will be displayed</param>
void iprintln(std::string strMsg)
{
    static const char *szPctS = "%s";
    strMsg = "e \"" + strMsg + "\"";
    const char *szMsg = strMsg.c_str();

    __asm
    {
        mov eax, szMsg
        push eax
        push szPctS
        xor eax, eax
        push eax
        call SV_GameSendServerCommand
        add esp, 0xc
    }
}