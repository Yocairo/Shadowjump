#include "pch.h"
#include <Windows.h>
#include <fstream>
#include <iostream>
#include "Application.h"
#include "JournalHandler.h"

using namespace std;

HHOOK hJournalHook = NULL;
HHOOK hGetMsgHook = NULL;

static fstream *pInputFile = nullptr;
static fstream *pOutputFile = nullptr;

static DWORD startTickCount = 0;
static int messageCount = 0;
static bool haveNextMessage = true;
static bool shouldMoveToNextMessage = true;
static EVENTMSG messageFromFile = {0}; // Will be obtained from the input file

/// <summary>
/// Set up the file IO required for Journal recording and playback
/// </summary>
/// <param name="szFilePath"></param>
bool setupFileIO(string strFilePath, bool asInput)
{
    // First-time memory allocation
    if (!pInputFile)
    {
        pInputFile = new fstream();
    }
    if (!pOutputFile)
    {
        pOutputFile = new fstream();
    }

    // Close any open file streams
    if (pInputFile && pInputFile->is_open())
    {
        pInputFile->close();
    }
    if (pOutputFile && pOutputFile->is_open())
    {
        pOutputFile->flush();
        pOutputFile->close();
    }

    // Set up the input file
    if (asInput)
    {
        pInputFile->open(strFilePath, ios::in | ios::binary); // Open in read mode and expect binary
    }
    else
    {
        pOutputFile->open(strFilePath, ios::out | ios::binary); // Open in write mode and expect binary
    }

    return true;
}

/// <summary>
/// Attempt to install the desired hook
/// </summary>
/// <param name="hookId">The hook type to install</param>
/// <param name="dllHandle">Handle to the DLL</param>
/// <param name="threadId">Thread id for the hook</param>
/// <returns>true if successfully installed</returns>
bool installJournalHook(int hookId, HMODULE dllHandle, DWORD threadId)
{
	if (hJournalHook != NULL)
	{
		cout << "Tried to start %d hook but a hook was still active" << hookId << endl;
		return false;
	}

	if (hookId == WH_JOURNALRECORD)
	{
		hJournalHook = SetWindowsHookEx(hookId, (HOOKPROC)JournalRecordProc, dllHandle, threadId);
	}
	else // Assume WH_JOURNALPLAYBACK
	{
		hJournalHook = SetWindowsHookEx(hookId, (HOOKPROC)JournalPlaybackProc, dllHandle, threadId);
	}

    if (hJournalHook == NULL)
    {
        cout << "hJournalHook error code: " << GetLastError() << endl;
        return false;
    }

	cout << hookId << " Hook successfully installed" << endl;
    return true;
}

/// <summary>
/// Attempt to uninstall the currently active JournalHook
/// </summary>
/// <returns>true if successfully uninstalled</returns>
bool uninstallJournalHook()
{
    bool result = true;
    if (hJournalHook)
    {
        if (UnhookWindowsHookEx(hJournalHook))
        {
            result = true;
			hJournalHook = NULL;
        }
        else
        {
            result = false;
        }
    }

	// Regardless of the hook type, we clear the start counter
    startTickCount = 0;
	messageCount = 0;
	haveNextMessage = true;
	shouldMoveToNextMessage = true;
	memset(&messageFromFile, 0, sizeof(messageFromFile));

    return result;
}

/// <summary>
/// Attempt to install GetMsgHook
/// </summary>
/// <param name="dllHandle">Handle to the DLL</param>
/// <param name="threadId">Thread Id for the hook</param>
/// <returns>true if successful, otherwise false</returns>
bool installGetMessageHook(HMODULE dllHandle, DWORD threadId)
{
    hGetMsgHook = SetWindowsHookEx(WH_GETMESSAGE, GetMsgProc, dllHandle, 0);

    if (hGetMsgHook == NULL)
    {
        cout << "hGetMsgHook error code: " << GetLastError() << endl;
        return false;
    }

    cout << "GetMsg hook successfully installed" << endl;
    return true;
}

/// <summary>
/// Attempt to uninstall GetMsgHook
/// </summary>
/// <returns>true if successful, otherwise false</returns>
bool uninstallGetMessageHook()
{
    bool result = true;
    if (hGetMsgHook)
    {
        if (UnhookWindowsHookEx(hGetMsgHook))
        {
            result = true;
            hGetMsgHook = NULL;
        }
        else
        {
            result = false;
        }
    }

    return result;
}

/// <summary>
/// Records messages the system removes from the system message queue
/// </summary>
/// <param name="code">Specifies how to process the message</param>
/// <param name="wParam">This parameter is not used</param>
/// <param name="lParam">A pointer to an EVENTMSG structure that contains the message to be recorded</param>
/// <returns></returns>
LRESULT CALLBACK JournalRecordProc(int code, WPARAM unused, LPARAM lParam)
{
    // If this is the first time this function is called, make note of the starting tick count
    if (startTickCount == 0)
    {
        startTickCount = GetTickCount();
    }

    SHORT stopRecordKeyState = GetKeyState(VK_STOP_RECORDING);
    if (stopRecordKeyState & 0x8000)
    {
        pOutputFile->flush(); // Flush now, because flushing after every write could affect performance
        pOutputFile->close(); // Close the file so we can use it for playback
        stopRecording();
        return CallNextHookEx(hJournalHook, code, unused, lParam);
    }

    // Only interested in actions
    if (code != HC_ACTION)
    {
        return CallNextHookEx(hJournalHook, code, unused, lParam);
    }

    // With this we see the time offset since the recording started, so that we can later play the actions back at the right moment
    // Although we need to make sure that this is not negative, which could happen if we are delayed somehow
    EVENTMSG *pEventMsg = (EVENTMSG *)lParam;
    pEventMsg->time = pEventMsg->time - startTickCount;
    if (pEventMsg->time < 0) pEventMsg->time = 0;

    // Write the entire EVENTMSG struct to our output file
    pOutputFile->write((const char *)pEventMsg, sizeof(*pEventMsg));

    return 0;
}

/// <summary>
/// Play back a series of mouse and keyboard messages recorded previously by the JournalRecordProc hook procedure
/// </summary>
/// <param name="code">A code the hook procedure uses to determine how to process the message</param>
/// <param name="wParam">This parameter is not used</param>
/// <param name="lParam">A pointer to an EVENTMSG structure that represents a message being processed by the hook procedure</param>
/// <returns></returns>
LRESULT CALLBACK JournalPlaybackProc(int code, WPARAM wParam, LPARAM lParam)
{
    // If this is the first time this function is called, make note of the starting tick count
    if (startTickCount == 0)
    {
        startTickCount = GetTickCount();
    }

	// Return without further processing
	if (code < 0)
	{
		return CallNextHookEx(hJournalHook, code, wParam, lParam);
	}

	// In case of HC_SKIP we can retrieve the next message
	if (code == HC_SKIP)
	{
		shouldMoveToNextMessage = true;
		messageCount++;
		return 0;
	}

	// Not HC_SKIP or HC_GETNEXT, play it safe and call next hook
	if (code != HC_GETNEXT)
	{
		return CallNextHookEx(hJournalHook, code, wParam, lParam);
	}

	// Now we arrived at HC_GETNEXT. The system can call this as many time as it wants and we'll still have to supply the same message.
	// When HC_SKIP is called we can advance to the next message in the file, which is what we do with shouldMoveToNextMessage

	EVENTMSG *pEventMsg = (EVENTMSG *)lParam;
	if (shouldMoveToNextMessage)
	{
		// We were not asked to repeat the message and have not yet reached EOF
		if (haveNextMessage)
		{
			// Read the next message from the file. If we don't have any, set haveNextMessage to false
			pInputFile->read((char *)&messageFromFile, sizeof(messageFromFile));
			if (pInputFile->eof())
			{
				haveNextMessage = false;
			}
		}
		else
		{
			// We've processed all messages there are
            cout << "Completed playback" << endl;
			(void)uninstallJournalHook();
			return 0;
		}
	}

	// Copy over the current message
	pEventMsg->hwnd = messageFromFile.hwnd;
	pEventMsg->message = messageFromFile.message;
	pEventMsg->paramH = messageFromFile.paramH;
	pEventMsg->paramL = messageFromFile.paramL;
    pEventMsg->time = startTickCount + messageFromFile.time;

    LRESULT ticksUntilNextMessage = pEventMsg->time - GetTickCount();
    return (ticksUntilNextMessage > 0) ? ticksUntilNextMessage : 0;
}

/// <summary>
/// 
/// </summary>
/// <param name="code"></param>
/// <param name="wParam"></param>
/// <param name="lParam"></param>
/// <returns></returns>
LRESULT CALLBACK GetMsgProc(int code, WPARAM wParam, LPARAM lParam)
{
	// This hook is needed to respond to "panic" keys (ctrl-alt-delete etc) which are used to stop recording/playback
	// When those keys are pressed, the system forcefully removes the hook. To make the application aware of this, we need to monitor for WM_CANCELJOURNAL messages

	// Call next hook without further processing
	if (code != HC_ACTION)
	{
		return CallNextHookEx(hGetMsgHook, code, wParam, lParam);
	}

	if (((MSG *)lParam)->message == WM_CANCELJOURNAL)
	{
		stopRecording();
		stopPlayback();
	}

	return 0;
}
