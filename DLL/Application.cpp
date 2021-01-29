#include "pch.h"
#include <iostream>
#include "Application.h"
#include "JournalHandler.h"
#include "TimeFunctionality.h"

#define EVENT_FILE_PATH     "R:\\Projects\\CoD4\\Shadowjump\\iw3mp_journaldump.evt"

using namespace std;

bool isRecordingActive; // Whether we're currently recording
bool isPlaybackActive;  // Whether we're currently playing back a recording

/// <summary>
/// Thread that does all the work in this DLL
/// </summary>
/// <param name="lpParameter">Parameters passed to this thread</param>
/// <returns>Always 0</returns>
DWORD CALLBACK ApplicationThread(LPVOID lpParameter)
{
    SDllThreadParams *pParams = (SDllThreadParams *)lpParameter;
    HMODULE dllHandle = pParams->dllHandle;
    delete pParams; // Caller is not able to free this memory, so we must do so

    // Allocate and redirect console so we can display debug output
    FILE *pNewConsole;
    AllocConsole();
    freopen_s(&pNewConsole, "CONOUT$", "w", stdout);
    freopen_s(&pNewConsole, "CONOUT$", "w", stderr);
    cout.clear();
    cerr.clear();

    // Improve the Windows timer resolution from ~16 ms to ~1 ms
    cout << "Improving timer resolution to 1 ms" << endl;
    if (improveTimerResolution())
    {
        // Set up necessary file IO
        cout << "Setting up file IO" << endl;
        if (setupFileIO(EVENT_FILE_PATH, false)) // As output first, because we have nothing to play back yet
        {
            // Try to install the GetMessageHook that we need in order to notice if playback has been terminated by use of ctrl-alt-delete/ctrl-escape
            // We can do this by adding a GetMessageHook that scans for WH_CANCELJOURNAL
            cout << "Installing GetMsgHook" << endl;
            if (installGetMessageHook(dllHandle, 0))
            {
                while (true)
                {
                    // TODO: Should probably check if CoD4 window is active, because we're using GetAsyncKeyState

                    if (isPlaybackActive)
                    {
                        // Nothing we can do. Only the JournalPlayback hook has the possibility to stop playback, since all other input is blocked
                        Sleep(100);
                        continue;
                    }

                    // Check if we need to start or stop recording
                    if (!isRecordingActive)
                    {
                        if (GetAsyncKeyState(VK_START_RECORDING) < 0)
                        {
                            // Start recording key is down. Wait until it's up before start recording as not to interfere with the JournalRecord hook
                            while (GetAsyncKeyState(VK_START_RECORDING) < 0)
                            {
                                Sleep(10);
                            }

                            // Attempt to install the hook that is responsible for recording input
                            if (!installJournalHook(WH_JOURNALRECORD, dllHandle, 0))
                            {
                                cout << "Failed to install JournalRecordHook" << endl;
                            }
                            else
                            {
                                cout << "Start recording" << endl;
                                isRecordingActive = true;
                            }
                        }
                        else if (GetAsyncKeyState(VK_START_PLAYBACK) < 0)
                        {
                            // Start playback key is down. Wait until it's up before we start playback as not to interfere with JournalPlayback hook
                            while (GetAsyncKeyState(VK_START_PLAYBACK) < 0)
                            {
                                Sleep(10);
                            }

                            // Attempt to install the hook that is responsible for playing back our recording
                            if (!installJournalHook(WH_JOURNALPLAYBACK, dllHandle, 0))
                            {
                                cout << "Failed to install JournalPlaybackHook" << endl;
                            }
                            else
                            {
                                cout << "Start recording" << endl;
                                isRecordingActive = true;
                            }
                        }
                    }
                    else if (isRecordingActive && (GetAsyncKeyState(VK_STOP_RECORDING) < 0))
                    {
                        // This is handled from within JournalRecord function, which is more reliable than checking if a key is pressed
                    }

                    Sleep(10);
                }

                (void)uninstallGetMessageHook();
            }
            else
            {
                cout << "Failed to install journal hook(s)" << endl;
            }
        }
        else
        {
            cout << "Failed to set up file IO" << endl;
        }

        resetTimerResolution();
    }
    else
    {
        cout << "Failed to improve timer resolution" << endl;
    }

    return 0;
}

/// <summary>
/// Stop recording
/// </summary>
void stopRecording()
{
    // We don't want to uninstall a non-existing hook
    if (!isRecordingActive)
    {
        return;
    }

    if (uninstallJournalHook())
    {
        cout << "Stopped recording" << endl;
        isRecordingActive = false;
    }
    else
    {
        cout << "Failed to stop recording" << endl;
    }
}

/// <summary>
/// Stop playback
/// </summary>
void stopPlayback()
{
    // We don't want to uninstall a non-existing hook
    if (!isPlaybackActive)
    {
        return;
    }

    if (uninstallJournalHook())
    {
        cout << "Stopped playback" << endl;
        isPlaybackActive = false;
    }
    else
    {
        cout << "Failed to stop playback" << endl;
    }
}
