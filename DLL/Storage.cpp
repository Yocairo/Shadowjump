#include "pch.h"

#include "Storage.h"
#include <string>
#include <fstream>
#include <ios>
#include <iostream>

Storage *Storage::s_pInstance = nullptr;

/// <summary>
/// Get pointer to instance of Singleton Storage class
/// </summary>
/// <returns>Pointer to Storage instance</returns>
Storage *Storage::getInstance()
{
    if (s_pInstance == nullptr)
    {
        s_pInstance = new Storage(STORAGE_FILE_PATH);
    }

    return s_pInstance;
}

Storage::Storage(string strOutputFilePath)
{
    m_outputFilePath = strOutputFilePath;
    m_pOutputFileStream = new ofstream();
}

/// <summary>
/// Flush all actions in timed actions map to the file
/// </summary>
void Storage::flush()
{
    if (m_timedActionsMap.empty())
    {
        cout << "Skipping flush for empty map" << endl;
        return;
    }

    m_pOutputFileStream->open(m_outputFilePath);

    DWORD actionsWritten = 0;
    for (auto &pair : m_timedActionsMap)
    {
        // Timeoffset,ActionType,rawactionvalue\n
        (*m_pOutputFileStream) << pair.first << "," << pair.second.type << "," << pair.second.rawValue << endl;
        actionsWritten++;
    }

    m_pOutputFileStream->close();
    cout << "Flushed " << actionsWritten << " actions" << endl;
}

/// <summary>
/// Clear the timed actions map to start a new recording
/// </summary>
void Storage::clear()
{
    if (m_timedActionsMap.size() > 0)
    {
        cout << "Clearing " << m_timedActionsMap.size() << " actions" << endl;
        m_timedActionsMap.clear();
    }
}

/// <summary>
/// Add an action at a specific time offset
/// </summary>
/// <param name="ts">Time offset compared to start of recording</param>
/// <param name="pAction">Pointer to SAction</param>
void Storage::add(ULONGLONG ts, SAction *pAction)
{
    m_timedActionsMap.insert(make_pair(ts, *pAction));
}