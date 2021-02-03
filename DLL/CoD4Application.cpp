#include "pch.h"

#include "CoD4Application.h"
#include <iostream>
#include "CoD4functions.h"
#include "Storage.h"
#include "TimeHelper.h"
#include "Hooks.h"

using namespace std;

CoD4Application *CoD4Application::s_pInstance = nullptr;

/// <summary>
/// Get the singleton instance
/// </summary>
/// <param name="dllInstance">Handle to current DLL</param>
/// <returns>Pointer to CoD4application instance</returns>
CoD4Application *CoD4Application::getInstance(HMODULE dllInstance)
{
    if (!s_pInstance)
    {
        s_pInstance = new CoD4Application(dllInstance);
    }

    return s_pInstance;
}

/// <summary>
/// Get the singleton instance
/// ONLY CALLABLE AFTER HAVING CALLED getInstance with HMODULE FIRST
/// </summary>
/// <param name="dllInstance">Handle to current DLL</param>
/// <returns>Pointer to CoD4application instance</returns>
CoD4Application *CoD4Application::getInstance()
{
    return s_pInstance;
}

/// <summary>
/// Commit sudoku
/// </summary>
void CoD4Application::exit()
{
    cout << "Unloading DLL" << endl;

    Hooks::getInstance()->uninstallHooks(); // Uninstall any hooks we may have added (keyboard, mouse)

    if (m_pNewConsoleOut)
    {
        fclose(m_pNewConsoleOut);
    }

    if (m_pNewConsoleErr)
    {
        fclose(m_pNewConsoleErr);
    }

    TimeHelper::resetTimerResolution(); // We improved the timer resolution, but we should undo that before stopping our DLL
    FreeConsole(); // Free the console we allocated
    FreeLibraryAndExitThread(m_dllInstance, 0); // Unload ourself :-(
}

/// <summary>
/// Get pointer to TimeHelper
/// </summary>
/// <returns></returns>
TimeHelper *CoD4Application::getTimeHelper()
{
    return m_pTimeHelper;
}

CoD4Application::CoD4Application(HMODULE dllInstance)
{
    m_pTimeHelper = new TimeHelper();
    m_pOutputSimulator = new OutputSimulator();
    m_pInputHandler = InputHandler::getInstance();
    m_pNewConsoleOut = nullptr; // Can't initialize the console here as this is called from DllMain
    m_pNewConsoleErr = nullptr;
    m_isRecording = false;
    m_isPlayingBack = false;
    m_dllInstance = dllInstance;
}

CoD4Application::~CoD4Application()
{
    delete m_pTimeHelper;
    delete m_pOutputSimulator;
}

/// <summary>
/// Check if recording is active
/// </summary>
/// <returns></returns>
bool CoD4Application::isRecording()
{
    return m_isRecording;
}

/// <summary>
/// Thread that does all the work in this DLL
/// </summary>
/// <param name="lpParameter">Parameters passed to this thread</param>
/// <returns>Always 0</returns>
void CoD4Application::run()
{
    // Allocate and redirect console so we can display debug output
    AllocConsole();
    freopen_s(&m_pNewConsoleOut, "CONOUT$", "w", stdout);
    freopen_s(&m_pNewConsoleErr, "CONOUT$", "w", stderr);
    cout.clear();
    cerr.clear();

    // Import actions from file if available
    (void)Storage::getInstance()->import();

    // Try to install the low level hook(s)
    cout << "Attempting to install hooks.." << endl;
    if (Hooks::getInstance()->installHooks(m_dllInstance, 0))
    {
        cout << "Successfully installed hooks" << endl;

        // Improve Windows timer resolution to 1 ms
        cout << "Attempt to improve timer resolution" << endl;
        if (TimeHelper::improveTimerResolution())
        {
            cout << "Changed timer resolution to 1 ms" << endl;

            MSG uMsg;
            while (GetMessage(&uMsg, NULL, 0, 0))
            {
                TranslateMessage(&uMsg);
                DispatchMessage(&uMsg);
            }
        }
        else
        {
            cout << "Failed to change timer resolution" << endl;
        }
    }
    else
    {
        cout << "Failed to install hooks" << endl;
    }
}

/// <summary>
/// Called when performed thread finishes playback
/// </summary>
void CoD4Application::onPlaybackFinished()
{
    m_isPlayingBack = false;
    iprintln("^4Finished ^7playback");
}

/// <summary>
/// Check if a key code is a function key for this CoD4Application
/// </summary>
/// <param name="keyCode">The key code to check</param>
/// <returns>true if key code is a function key and handleFunctionKey should be called</returns>
bool CoD4Application::isFunctionKey(UINT keyCode)
{
    // F1 = start recording
    // F2 = stop recording
    // F3 = start playback
    // F4 = stop playback
    // F5 = unload DLL
    if ((keyCode >= VK_F1) && (keyCode <= VK_F5))
    {
        return true;
    }

    return false;
}

/// <summary>
/// Handle a function key action
/// </summary>
/// <param name="wParam">The action (WM_KEYDOWN, WM_KEYUP, WM_SYSKEYDOWN, WM_SYSKEYUP)</param>
/// <param name="keyCode">The virtual key code (VK_F1, etc)</param>
void CoD4Application::handleFunctionKey(WPARAM wParam, UINT keyCode)
{
    if (keyCode == VK_F5) // Unload DLL key
    {
        exit();
        return;
    }
    else if (keyCode == VK_F1) // Start recording
    {
        if (!m_isRecording)
        {
            Storage::getInstance()->clear();
            Action::resetInputStates();
            m_pTimeHelper->setStartTime();
            m_isRecording = true;
            iprintln("^2Started ^7recording");
        }
    }
    else if (keyCode == VK_F2) // Stop recording
    {
        if (m_isRecording)
        {
            m_isRecording = false;
            Storage::getInstance()->flush();
            iprintln("^1Stopped ^7recording");
        }
    }
    else if (keyCode == VK_F3) // Start playback
    {
        if (!m_isPlayingBack)
        {
            m_isPlayingBack = true;
            m_pOutputSimulator->playback();
            iprintln("^5Started ^7playback");
        }
    }
    else if (keyCode == VK_F4) // Stop playback
    {
        if (m_isPlayingBack)
        {
            // TODO: stop playback
            //m_isPlayingBack = false;
            //iprintln("^Stopped ^7playback");
        }
    }
    else
    {
        cout << keyCode << " is not a function key" << endl;
    }
}