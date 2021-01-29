#pragma once

#include <Windows.h>
#include <string>

bool setupFileIO(std::string, bool);
bool installJournalHook(int, HMODULE, DWORD);
bool uninstallJournalHook();
bool installGetMessageHook(HMODULE, DWORD);
bool uninstallGetMessageHook();
LRESULT CALLBACK JournalRecordProc(int, WPARAM, LPARAM);
LRESULT CALLBACK JournalPlaybackProc(int, WPARAM, LPARAM);
LRESULT CALLBACK GetMsgProc(int, WPARAM, LPARAM);
