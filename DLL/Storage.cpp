#include "pch.h"

#include "Storage.h"
#include <string>
#include <fstream>
#include <ios>
#include <iostream>
#include <vector>

#define STORAGE_FILE_DELIMITER      ","
#define STORAGE_FILE_PARTS_COUNT    3       // timeOffset and ActionType and rawActionValue

Storage *Storage::s_pInstance = nullptr;
multimap<UINT, SAction> Storage::s_timedActionsMap;

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
    m_filePath = strOutputFilePath;
    m_pFileStream = new fstream();
}

/// <summary>
/// Split a delimitered string into a vector
/// Courtesty of https://stackoverflow.com/a/46931770
/// </summary>
/// <param name="line"></param>
/// <param name="delimiter"></param>
/// <returns></returns>
vector<string> Storage::split(string s, string delimiter)
{
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    string token;
    vector<string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != string::npos)
    {
        token = s.substr(pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back(token);
    }

    res.push_back(s.substr(pos_start));
    return res;
}

/// <summary>
/// Import all actions from the storage file into the timed actions map to prepare for playback
/// </summary>
void Storage::import()
{
    clear(); // Clear timed actions map

    // Try to open it. If it doesn't exist, we're done here
    m_pFileStream->open(m_filePath, ios::in);
    if (!*m_pFileStream)
    {
        cout << "No timed actions file on disk" << endl;
        return;
    }

    string line;
    while (!m_pFileStream->eof())
    {
        // Get the complete line: timeOffset,ActionType,rawActionValue
        getline(*m_pFileStream, line);
        
        // Get each part (timeOffset and ActionType and rawActionValue)
        vector<string> parts = split(line, STORAGE_FILE_DELIMITER);
        if (parts.size() == STORAGE_FILE_PARTS_COUNT)
        {
            try
            {
                SAction action;
                action.type = static_cast<ActionType>(stoi(parts[1]));
                action.rawValue = stoull(parts[2]);
                s_timedActionsMap.insert(make_pair(stoul(parts[0]), action));
            }
            catch (int e)
            {
                cout << "Exception " << e << " occurred while parsing entry: '" << line << "'" << endl;
            }
        }
        else
        {
            cout << "Skipping invalid entry: '" << line << "'" << endl;
        }
    }

    m_pFileStream->close();
    cout << "Imported " << s_timedActionsMap.size() << " actions" << endl;
}

/// <summary>
/// Flush all actions in timed actions map to the file
/// </summary>
void Storage::flush()
{
    if (s_timedActionsMap.empty())
    {
        cout << "Skipping flush for empty map" << endl;
        return;
    }

    m_pFileStream->open(m_filePath, ios::out);

    DWORD actionsWritten = 0;
    for (auto &pair : s_timedActionsMap)
    {
        // Timeoffset,ActionType,rawactionvalue\n
        (*m_pFileStream) << pair.first << STORAGE_FILE_DELIMITER << pair.second.type << STORAGE_FILE_DELIMITER << pair.second.rawValue << endl;
        actionsWritten++;
    }

    m_pFileStream->close();
    cout << "Flushed " << actionsWritten << " actions" << endl;
}

/// <summary>
/// Clear the timed actions map to start a new recording
/// </summary>
void Storage::clear()
{
    if (s_timedActionsMap.size() > 0)
    {
        cout << "Clearing " << s_timedActionsMap.size() << " actions" << endl;
        s_timedActionsMap.clear();
    }
}

/// <summary>
/// Add an action at a specific time offset
/// </summary>
/// <param name="ts">Time offset compared to start of recording</param>
/// <param name="pAction">Pointer to SAction</param>
void Storage::add(UINT ts, SAction *pAction)
{
    s_timedActionsMap.insert(make_pair(ts, *pAction));
}